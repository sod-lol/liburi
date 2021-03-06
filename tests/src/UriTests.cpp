#ifndef URI_TESTS
#define URI_TESTS

#include <stdio.h>
#include <Uri/Uri.hpp>
#include <gtest/gtest.h>

TEST(UriTests, ParseAndGetInfoTest1) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("https://google.com/ops?foo=bar&foo1=bar1"));
    ASSERT_EQ(uri.getSchema(), "https");
    ASSERT_EQ(uri.getUserInfo(), "");
    ASSERT_EQ(uri.getHost(), "google.com");
    ASSERT_EQ(uri.getPort(), 443);
    ASSERT_EQ(uri.getPath(), (std::vector< std::string > {
        "ops"
    }));
    ASSERT_EQ(uri.getQueryParams(), (std::map< std::string, std::string > {
        {"foo", "bar"},
        {"foo1", "bar1"}
    }));
}

TEST(UriTests, ParseAndGetInfoTest2) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top"));
    ASSERT_EQ(uri.getSchema(), "https");
    ASSERT_EQ(uri.getUserInfo(), "john.doe");
    ASSERT_EQ(uri.getHost(), "www.example.com");
    ASSERT_EQ(uri.getPort(), 123);
    ASSERT_EQ(uri.getPath(), (std::vector< std::string > {
        "forum",
        "questions",
        ""
    }));
    ASSERT_EQ(uri.getQueryParams(), (std::map< std::string, std::string > {
        {"tag", "networking"},
        {"order", "newest"}
    }));
    ASSERT_EQ(uri.getFragment(), "top");
}

TEST(UriTests, ParseAndGetInfoTest3) {
    ASSERT_TRUE(true);
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("mailto:John.Doe@example.com"));
    ASSERT_EQ(uri.getSchema(), "mailto");
    ASSERT_EQ(uri.getPath(), (std::vector< std::string > {
        "John.Doe@example.com"
    }));
}

TEST(UriTests, ParseTwiceFirstWithPortAndQueryAndFragmentSecondWithoutThese) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top"));
    ASSERT_TRUE(uri.parseFromString("https://www.example.com"));
    ASSERT_EQ(uri.getSchema(), "https");
    ASSERT_EQ(uri.getPath(), (std::vector< std::string > {
        
    }));
    ASSERT_EQ(uri.getPort(), 443);
    ASSERT_EQ(uri.getQueryParams(), (std::map< std::string, std::string > {
        
    }));
}

TEST(UriTests, ParseUriWithValidPortButWierdType) {
    Uri::Uri uri;
    ASSERT_TRUE(uri.parseFromString("https://www.example.com:00000123/forum/questions/?tag=networking&order=newest#top"));
    ASSERT_TRUE(uri.hasPort());
    ASSERT_EQ(uri.getPort(), 123);
}

TEST(UriTests, ParseUriWithInvalidPort) {
    Uri::Uri uri;
    ASSERT_FALSE(uri.parseFromString("https://www.example.com:123s/forum/questions/?tag=networking&order=newest#top"));
    ASSERT_FALSE(uri.hasPort());
}

TEST(UriTests, ParseUriWithBadport) {
    Uri::Uri uri;
    ASSERT_FALSE(uri.parseFromString("https://www.example.com:65536/forum/questions/?tag=networking&order=newest#top"));
    ASSERT_FALSE(uri.hasPort());
}

TEST(UriTests, ParseUriWithRelativeAndFullPath) {
    Uri::Uri uri;
    typedef struct {
        std::string uri;
        bool isRelative;
    } uriRelativeTest;
    std::vector< uriRelativeTest > tests {
        {"/foo", true},
        {"foo/", true},
        {"/foo/bar/hoo/loo", true},
        {"foo/bar/hoo/loo", true},
        {"https://www.example.com/foo/bar", false}
    };
    int index = 0;
    for (auto &test : tests) {
        ASSERT_TRUE(uri.parseFromString(test.uri)) << index;
        ASSERT_EQ(uri.isRelative(), test.isRelative) << index;
        index++;
    }
}

TEST(UriTests, GetPathOfRelativePath) {
    Uri::Uri uri;
    typedef struct {
        std::string uri;
        std::vector< std::string > path;
    } relativePath;
    std::vector< relativePath > tests {
        {"/foo", std::vector< std::string > {"", "foo"}},
        {"foo/", std::vector< std::string > {"foo", ""}},
        {"/foo/", std::vector< std::string > {"", "foo", ""}},
        {"/foo/bar/hoo/loo", std::vector< std::string > {"", "foo", "bar", "hoo", "loo"}},
        {"foo/bar/hoo/loo/", std::vector< std::string > {"foo", "bar", "hoo", "loo", ""}},
        {"https://www.example.com/foo/bar", std::vector< std::string > {"foo", "bar"}}
    };
    int index = 0;
    for (auto &test : tests) {
        ASSERT_TRUE(uri.parseFromString(test.uri)) << index;
        ASSERT_EQ(uri.getPath(), test.path) << index;
        index++;
    }
}

TEST(UriTests, GetFragmentOfRelativePath) {
    Uri::Uri uri;
    typedef struct {
        std::string uri;
        std::string fragment;
    } relativePath;
    std::vector< relativePath > tests {
        {"/foo#bar", "bar"},
        {"foo/#bar2", "bar2"},
        {"/foo/#foo", "foo"},
        {"/foo/bar/hoo/loo#bar", "bar"},
        {"foo/bar/hoo/loo/#bar1", "bar1"},
        {"https://www.example.com/foo/bar#bar", "bar"}
    };
    int index = 0;
    for (auto &test : tests) {
        ASSERT_TRUE(uri.parseFromString(test.uri)) << index;
        ASSERT_EQ(uri.getFragment(), test.fragment) << index;
        index++;
    }
}

TEST(UriTests, GetQueryParamsOfRelativePath) {
    Uri::Uri uri;
    typedef struct {
        std::string uri;
        std::map< std::string, std::string > querys;
    } relativePath;
    std::vector< relativePath > tests {
        {"/foo?test=bar&test1", std::map< std::string, std::string > {{"test", "bar"}, {"test1", ""}}},
        {"foo/?test2=bar1&tar=tir#bar2", std::map< std::string, std::string > {{"test2", "bar1"}, {"tar", "tir"}}},
        {"/foo/?test2&tar=tir#foo", std::map< std::string, std::string > {{"test2", ""}, {"tar", "tir"}}},
        {"/foo/bar/hoo/loo?foo#bar", std::map< std::string, std::string > {{"foo", ""}}},
        {"foo/bar/hoo/loo/#bar1", std::map< std::string, std::string > {}},
        {"https://www.example.com/foo/bar?foo=bar#bar", std::map< std::string, std::string > {{"foo", "bar"}}}
    };
    int index = 0;
    for (auto &test : tests) {
        ASSERT_TRUE(uri.parseFromString(test.uri)) << index;
        ASSERT_EQ(uri.getQueryParams(), test.querys) << index;
        index++;
    }
}

TEST(UriTests, InvalidPortTest) {
    Uri::Uri uri;
    typedef struct {
        std::string uri;
        bool isTrue;
    } ports;
    std::vector< ports > tests {
        {"http://www.example:s/foo/do", false},
        {"https://www.example:33/foo/do", true},
        {"https://www.example:333213/foo/do", false},
        {"https://www.example:spam/bar/foo", false},
        {"https://loca.com:432", true},
        {"https://www.example.com/foo/bar?foo=bar#bar", true}
    };
    int index = 0;
    for (auto &test : tests) {
        ASSERT_EQ(uri.parseFromString(test.uri), test.isTrue) << index;
        index++;
    }
}

TEST(UriTests, HostInvalidAndValidTest) {
    Uri::Uri uri;
    typedef struct {
        std::string uri;
        bool isTrue;
    } hosts;
    std::vector< hosts > tests {
        {"h://www.example:32/foo/do", true},
        {"4://www.example:33/foo/do", false},
        {"xo://www.example:11/foo/do", true},
        {"x-+.5://www.example:31/bar/foo", true},
        {"ha0://loca.com:432", true},
        {"0://loca.com:432", false},
        {"fu9azAZ0://loca.com:432", true},
        {"+://www.example.com/foo/bar?foo=bar#bar", false},
        {"htt^ps://www.example.com/foo/bar?foo=bar#bar", false},
        {"htt+ps://www.example.com/foo/bar?foo=bar#bar", true},
        {"h-t+ps://www.example.com", true},
        {"ht438+34j-://www.example.com/foo/bar?foo=bar#bar", true}
    };
    int index = 0;
    for (auto &test : tests) {
        ASSERT_EQ(uri.parseFromString(test.uri), test.isTrue) << index;
        index++;
    }
}

TEST(UriTests, HostIPLiteralAndRegName) {
    Uri::Uri uri;
    typedef struct {
        std::string uri;
        std::string host;
    } hosts;
    std::vector< hosts > tests {
        {"h://www.example:32/foo/do", "www.example"},
        {"https://192.168.2.2/foo/bar", "192.168.2.2"},
        {"xo://www.foobar.org:11/foo/do", "www.foobar.org"},
        {"x-+.5://foobar.org:31/bar/foo", "foobar.org"},
        {"ha0://loca.org.ta.com:432", "loca.org.ta.com"},
        {"https://10.1.2.255:432", "10.1.2.255"},
        {"ldap://[2001:db8::7]/c=GB?objectClass?one", "2001:db8::7"},
        {"ldap://[2001:db8::7]:8080/c=GB?objectClass?one", "2001:db8::7"},
        {"ldap://[v8.2001:db8::7]/c=GB?objectClass?one", "v8-2001:db8::7"}
    };
    int index = 0;
    for (auto &test : tests) {
        ASSERT_TRUE(uri.parseFromString(test.uri)) << index;
        ASSERT_EQ(uri.getHost(), test.host) << index;
        index++;
    }
}

#endif