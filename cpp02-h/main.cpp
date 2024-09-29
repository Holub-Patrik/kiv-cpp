#include <cstdio>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

class RAII_File
{
	public:
    FILE * file;
    bool state;

     RAII_File(std::string path, std::string mode) {
      // load the file and set status depending on outcome of fopen()
      file = fopen(path.c_str(), mode.c_str());
      state = file != NULL;
    }

    ~RAII_File() {
      int failed = fclose(file);
      if (failed) {
        std::cout << "Error while closing the file" << std::endl;
      }
    }

		std::string Read_Line()
		{
      std::string line = "";

      if (!state) {
        return line;  
      }

      for (char c = fgetc(file); c != '\n' && c != EOF; c = fgetc(file)) {
        line.push_back(c);
      }

			return line;
		}

		void Write_Line(const std::string& line)
		{
      std::string line_to_write = line + "\n";
      int failed = fprintf(file, "%s", line_to_write.c_str());
		}
};

void Generate_Files(std::vector<std::string> filenames)
{
  std::vector<std::unique_ptr<RAII_File>> files;

  for (const auto filename : filenames) {
    auto file = std::make_unique<RAII_File>(filename, "w");

    if (!file->state) {
      std::cout << "Error while opening file" << std::endl;
      continue;
    } else {
      files.push_back(std::move(file));
    }
  }

  int i = 0;
  for (auto &file : files) {
    std::string line = "This is file: ";
    line += std::to_string(i);
    file->Write_Line(line);
    i++;
  }
}

bool Verify_Files(std::vector<std::string> filenames)
{
  std::vector<std::unique_ptr<RAII_File>> files;

  for (const auto filename : filenames) {
    auto file = std::make_unique<RAII_File>(filename, "r");

    if (!file->state) {
      std::cout << "Error while opening file" << std::endl;
      continue;
    }

    files.push_back(std::move(file));
  }

  int i = 0;
  for (auto &file : files) {
    std::string ref_line = "This is file: ";
    ref_line += std::to_string(i);
    auto line = file->Read_Line();
    if (line != ref_line) {
      return false;
    }
    i++;
  }

	return true;
}

int main(int argc, char** argv)
{
  std::vector<std::string> filenames{"file1.txt", "file2.txt", "file3.txt"};
	Generate_Files(filenames);

	const bool outcome = Verify_Files(filenames);

	if (outcome)
		std::cout << "Vse probehlo v poradku" << std::endl;
	else
		std::cerr << "Vyskytla se chyba!" << std::endl;

	return 0;
}
