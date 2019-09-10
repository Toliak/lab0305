#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <forward_list>

namespace filesystem = boost::filesystem;

struct BrokerFile
{
    using DirsType = std::forward_list<std::string>;

    std::string filename;
    DirsType directories;
};

class BrokerResolver
{
public:
    using Collection = std::forward_list<BrokerFile>;
    static boost::regex filenameChecker;

    // Рекурсивный резольвер симлинков
    static filesystem::path resolveIfSymlink(const filesystem::path &path)
    {
        if (!filesystem::is_symlink(path)) {
            return path;
        }

        return resolveIfSymlink(
            filesystem::read_symlink(path)
        );
    }

    // Запомнить информацию о файле, если он удовлетворяет условию
    void saveFileIfCorrect(const filesystem::path &path)
    {
        std::string filename = path.filename().string();
        if (boost::regex_match(filename, filenameChecker)) {

        }

        fileCollection.emplace_front(BrokerFile{path.filename().string(), currentDirectories});
    }

    void resolve(const filesystem::path &path)
    {
        for (const filesystem::directory_entry &entry : filesystem::directory_iterator{path}) {
            std::cout << entry.path() << std::endl;

            if (filesystem::is_directory(resolveIfSymlink(entry))) {
                currentDirectories.push_front(
                    filesystem::path{entry}.stem().string()
                );
                resolve(entry);
            } else if (filesystem::is_regular_file(entry)) {
                saveFileIfCorrect(entry);
            }
        }

        if (!currentDirectories.empty()) {
            currentDirectories.pop_front();
        }
    }

    [[nodiscard]] const Collection &getFileCollection() const
    {
        return fileCollection;
    }
private:
    BrokerFile::DirsType currentDirectories;
    Collection fileCollection;
};

boost::regex BrokerResolver::filenameChecker{R"((\w+)_(\d{8})_([12]\d{3}(0[1-9]|1[0-2])(0[1-9]|[12]\d|3[01])).(\w+))"};

int main(int argc, char *argv[])
{
    filesystem::path path = ".";
    if (argc > 1) {
        path = argv[1];
    }

    BrokerResolver resolver{};
    resolver.resolve(path);

    return 0;
}