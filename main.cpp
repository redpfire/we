
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <cstdint>
#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace fs=boost::filesystem;
using namespace std;

static FILE *file;
static uint32_t filecount;
static char *version;
static uint32_t ds_ptr;
static uint32_t f_pos;
static uint32_t c_pos;

typedef struct
{
    char *name;
    uint32_t offset;
    uint32_t size;
} file_t;

char *read_str()
{
    uint32_t size;
    fread(&size, 4, 1, file);
    char *buf = (char *)malloc(size+1);
    memset(buf, 0, sizeof(buf));
    fread(buf, size, 1, file);
    return buf;
}

void write_str(const char *str)
{
    uint32_t size;
    size = strlen(str);
    fwrite(&size, 4, 1, file);
    fwrite(str, 1, size, file);
}

void read_header()
{
    version = read_str();
    filecount = 0;
    fread(&filecount, 4, 1, file);
    if(strcmp(version, "PKGV0001") != 0)
    {
        printf("File not compatible. Exiting.\n");
        fclose(file);
        exit(1);
    }
    printf("PKGV0001 - %d files\n", filecount);
}

file_t *read_files()
{
    file_t *files = (file_t *)malloc(sizeof(file_t) * filecount+1);

    for(int i = 0 ; i < filecount; ++i)
    {
        files[i].name = read_str();
        fread(&files[i].offset, 4, 1, file);
        fread(&files[i].size, 4, 1, file);
        ds_ptr = ftell(file);
    }
    printf("data_structure pointer: %x\n", ds_ptr);
    return files;
}

int read_tmp_paths(char **dst, const char *tmpdir)
{
    int i = 0;
    for(auto& p: fs::recursive_directory_iterator(fs::path(tmpdir)))
    {
        if(!fs::is_directory(p.path()))
        {
            string s(p.path().string());
            char *path = (char *)malloc(s.length()+1);
            strcpy(path, s.c_str());
            ds_ptr += 2 + s.length();
            dst[i] = path;
            ++i;
        }
    }
    ds_ptr += 16;
    return i;
}

void inflate_files(char **fnames, size_t n)
{
    f_pos = 0;
    uint8_t *buf;
    uint32_t fsize;
    for(int i = 0 ; i < n; ++i)
    {
        string path(fnames[i]);
        vector<string> path_v;

        boost::split(path_v, path, boost::is_any_of("/"));
        path_v.erase(path_v.begin());
        path = boost::join(path_v, "/");

        fsize = fs::file_size(fs::path(fnames[i]));

        printf("Inflating %s (%d)\n", path.c_str(), fsize);

        fseek(file, c_pos, SEEK_SET);
        write_str(path.c_str());
        fwrite(&f_pos, 4, 1, file);
        fwrite(&fsize, 4, 1, file);
        c_pos = ftell(file);

        buf = (uint8_t*)malloc(fsize);
        fseek(file, ds_ptr + f_pos, SEEK_SET);
        FILE *f = fopen(fnames[i], "rb");
        memset(buf, 0, fsize);
        fread(buf, 1, fsize, f);
        fwrite(buf, 1, fsize, file);

        fclose(f);
        f_pos += fsize;
        free(buf);
    }
    
}

void prepare_header(const char *tmpdir)
{
    char **paths = (char**)malloc(128 * sizeof(char*));
    ds_ptr = 0;
    uint32_t sz = read_tmp_paths(paths, tmpdir);
    write_str("PKGV0001");
    fwrite(&sz, 4, 1, file);
    c_pos = ftell(file);
    inflate_files(paths, sz);
}

void prepare_file(const char *filename)
{
    file = fopen(filename, "r+b");
    fseek(file, 0, SEEK_SET);
    read_header();
}

int main(int argc, char **argv)
{
    if(argc < 2)
        return 1;

    if(strcmp(argv[1], "x") == 0) // unpack
    {
        prepare_file(argv[2]);
        file_t *files = read_files();
        vector<string> filename_v;
        boost::split(filename_v, argv[2], boost::is_any_of("."));
        string tmppath("tmp_");
        tmppath += *filename_v.begin();
        if(fs::is_directory(fs::path(tmppath)))
            fs::remove_all(fs::path(tmppath));
        fs::create_directories(fs::path(tmppath));
        for(int i = 0; i < filecount; ++i)
        {
            printf("%x:\nunpacking %s (%d)\n", files[i].offset, files[i].name, files[i].size);
            string name(files[i].name);
            vector<string> tokens;
            boost::split(tokens, name, boost::is_any_of("/"));
            string token;
            string path(tmppath.c_str());
            path += "/";
            for(vector<string>::iterator tok_iter = tokens.begin();
                tok_iter != tokens.end(); ++tok_iter)
            {
                if(distance(tok_iter, tokens.end()) > 1)
                {
                    token = *tok_iter;
                    path += token;
                    path += "/";
                    fs::create_directories(fs::path(path.c_str()));
                }
            }
            path = string(tmppath.c_str());
            path += "/";
            path += files[i].name;
            FILE *f = fopen(path.c_str(), "wb");
            fseek(file, files[i].offset + ds_ptr, SEEK_SET);
            unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * files[i].size);
            fread(buffer, sizeof(unsigned char), files[i].size, file);
            fwrite(buffer, sizeof(unsigned char), files[i].size, f);
            fclose(f);
            free(buffer);
        }
        fclose(file);
    }
    else if(strcmp(argv[1], "r") == 0)
    {
        vector<string> filename_v;
        boost::split(filename_v, argv[2], boost::is_any_of("."));
        string tmppath("tmp_");
        tmppath += *filename_v.begin();
        if(!fs::is_directory(fs::path(tmppath)))
        {
            printf("No temp folder for that file! Exiting.\n");
            return 1;
        }
        fs::remove_all(argv[2]);
        file = fopen(argv[2], "wb");
        prepare_header(tmppath.c_str());
        fclose(file);
    }
    else if(strcmp(argv[1], "l") == 0)
    {
        prepare_file(argv[2]);
        file_t *files = read_files();
        for(int i = 0; i < filecount; ++i)
            printf("\t%s (%d)\n", files[i].name, files[i].size);
        fclose(file);
    }
    else if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printf("%s [command] [args]\n\n" \
               "commands:\n" \
               "\tx <file> - extract\n" \
               "\tl <file> - list\n" \
               "\tr <file> - repack\n" \
               "\n", argv[0]);
        return 0;
    }
    else
    {
        printf("No such instruction. Run --help.\n");
        return 1;
    }

    return 0;
}