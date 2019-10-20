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

TEST(BrokerResolver, SetIfMoreBad)
{
    int left = 9;
    BrokerResolver::setIfMore(left, 5);
    EXPECT_EQ(left, 9);
}

TEST(BrokerResolver, SetIfMore)
{
    int left = 9;
    BrokerResolver::setIfMore(left, 10);
    EXPECT_EQ(left, 10);
}

class BrokerResolverFixture: public ::testing::Test
{
protected:
    BrokerResolver resolver;
    BrokerResolver::DataCollection data;
    BrokerResolver::Collection files;

    void SetUp() override
    {
        resolver.resolve("../tests/data");
        files = resolver.getFileCollection();
        data = resolver.getDataCollection();
    }
};

TEST_F(BrokerResolverFixture, GetFiles)
{
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

TEST_F(BrokerResolverFixture, GetFilesNestedDir)
{
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

TEST_F(BrokerResolverFixture, GetFilesSymlinkFile)
{
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

TEST_F(BrokerResolverFixture, GetFilesSymlinkDir)
{
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

TEST_F(BrokerResolverFixture, GetFilesNoOld)
{
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


TEST_F(BrokerResolverFixture, DataCollection)
{
    auto iterator = data.find({"dir", 1234});
    ASSERT_NE(iterator, data.cend());

    EXPECT_EQ(iterator->second.files, 3);
    EXPECT_EQ(iterator->second.lastDate,"20181010");
}

TEST_F(BrokerResolverFixture, DataCollectionNested)
{
    auto iterator = data.find({"dir nested_dir", 1234});
    ASSERT_NE(iterator, data.cend());

    EXPECT_EQ(iterator->second.files, 1);
    EXPECT_EQ(iterator->second.lastDate,"20181005");
}

TEST_F(BrokerResolverFixture, DataCollectionSymlink)
{
    auto iterator = data.find({"new_dir", 1234});
    ASSERT_NE(iterator, data.cend());

    EXPECT_EQ(iterator->second.files, 3);
    EXPECT_EQ(iterator->second.lastDate,"20181010");
}

TEST_F(BrokerResolverFixture, DataCollectionWrongExtension)
{
    EXPECT_EQ(
        std::find(
            files.cbegin(),
            files.cend(),
            BrokerFile{
                "balance_00001459_20191011.yml",
                {"dir", "nested_dir"}
            }
        ),
        files.cend()
    );
}