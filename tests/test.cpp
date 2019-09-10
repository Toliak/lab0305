#include <gtest/gtest.h>
#include "BrokerResolver.h"

TEST(BrokerFile, DirectoriesToString)
{
    BrokerFile brokerFile{
        "filename",
        {"dir1", "dir2"}
    };

    EXPECT_EQ(brokerFile.directoriesToString(), "dir1 dir2");
}

TEST(BrokerFile, Equal)
{
    BrokerFile brokerFile{
        "filename",
        {"dir1", "dir2"}
    };
    BrokerFile equalBrokerFile{
        "filename",
        {"dir1", "dir2"}
    };
    BrokerFile notEqualBrokerFile{
        "filename",
        {"dir1", "dir2", "dir3"}
    };

    EXPECT_EQ(brokerFile, equalBrokerFile);
    EXPECT_EQ(brokerFile == notEqualBrokerFile, false);
}

TEST(BrokerDataPair, Equal)
{
    BrokerData::Pair pair{"broker", 500};
    BrokerData::Pair equalPair{"broker", 500};
    BrokerData::Pair notEqualPair{"_____", 500};

    EXPECT_EQ(pair, equalPair);
    EXPECT_EQ(pair == notEqualPair, false);
}

TEST(BrokerDataPair, Hash)
{
    BrokerData::Pair pair{"broker", 500};
    size_t hash = BrokerData::Pair::Hash{}(pair);

    EXPECT_NE(hash, 0);
}

TEST(BrokerResolver, TestGetFiles)
{
    BrokerResolver resolver{};
    resolver.resolve("../tests/data");

    auto files = resolver.getFileCollection();

    EXPECT_NE(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001234_20181001.txt",
                {"dir"}
            }
        ),
        files.cend()
    );
    EXPECT_NE(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001234_20181007.txt",
                {"dir"}
            }
        ),
        files.cend()
    );
}

TEST(BrokerResolver, TestGetFilesNestedDir)
{
    BrokerResolver resolver{};
    resolver.resolve("../tests/data");

    auto files = resolver.getFileCollection();

    EXPECT_NE(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001234_20181005.txt",
                {"dir", "nested_dir"}
            }
        ),
        files.cend()
    );
}

TEST(BrokerResolver, TestGetFilesSymlinkFile)
{
    BrokerResolver resolver{};
    resolver.resolve("../tests/data");

    auto files = resolver.getFileCollection();

    EXPECT_NE(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001234_20181010.txt",
                {"dir"}
            }
        ),
        files.cend()
    );
}

TEST(BrokerResolver, TestGetFilesSymlinkDir)
{
    BrokerResolver resolver{};
    resolver.resolve("../tests/data");

    auto files = resolver.getFileCollection();

    EXPECT_NE(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001234_20181001.txt",
                {"new_dir"}
            }
        ),
        files.cend()
    );
    EXPECT_NE(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001234_20181007.txt",
                {"new_dir"}
            }
        ),
        files.cend()
    );
}

TEST(BrokerResolver, TestGetFilesNoOld)
{
    BrokerResolver resolver{};
    resolver.resolve("../tests/data");

    auto files = resolver.getFileCollection();

    EXPECT_EQ(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001235_20190505.old.txt",
                {"dir"}
            }
        ),
        files.cend()
    );
    EXPECT_EQ(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001235_20190505.old.txt",
                {"new_dir"}
            }
        ),
        files.cend()
    );
}
