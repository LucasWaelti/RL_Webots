#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string.h>
#include <sstream>
#include <sys/stat.h>
#include <vector>

/**
 * Create a csv file in the specified folder if it does not exist yet. 
 * @param dir (std::string) Path to the file. 
 * @param file (std::string) The file name must include the path to it. 
 * @param headers (std::vector<std::string>) Name of the columns. 
 */
int create_log(std::string dir, std::string file, std::vector<std::string> headers);

/**
 * Write a line into the specified file (path to file included).
 * Each element of the vector will be written on the same line, 
 * separated by a comma. 
 * @param file (std::string) the file name must include the path to it. 
 * @param values (std::vector<std::string>) vector containing the values to write. 
 */
void write_line(std::string file, std::vector<std::string> values);

/**
 * Write a line into the specified file (path to file included).
 * Each element of the vector will be written on the same line, 
 * separated by a comma. 
 * @overload void write_line(std::string file, std::vector<std::string> values);
 * @param file (std::string) the file name must include the path to it. 
 * @param values (std::vector<double>) vector containing the values to write.
 * The values are converted to std::string.  
 */
void write_line(std::string file, std::vector<double> values);

/**
 * Read the value of the last stored rollout. Defaults to 0 if no file
 * or if no rollout was previously logged. 
 * @param file (std::string) the file name must include the path to it. 
 * @output (int) value of the last rollout. 
 */
int read_last_rollout(std::string file);

/**
 * Read the index of the last stored particle. Defaults to 0 if no file
 * or if no particle was previously logged. 
 * @param file (std::string) the file name must include the path to it. 
 * @output (int) index of the last logged particle. 
 */
int read_last_particle(std::string file);