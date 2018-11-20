#pragma once


const char* platform_file_basename(const char* filename);

bool platform_file_rename(const char* existing_file, const char* new_file);
bool platform_file_delete(const char* filename);

bool platform_pathfile_exists(const char* path);



