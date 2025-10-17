//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2025 Gennaro Prota
// 
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars

#include "path.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

namespace boost::handlebars {

void
convert_to_slash(
    std::filesystem::path & path,
    PathStyle style)
{
#ifdef _WIN32
    if (style == PathStyle::native || style == PathStyle::windows)
    {
        std::string str = path.string(); // native format
        std::replace(str.begin(), str.end(), '\\', '/');
        path = std::filesystem::path(str);
    }
#endif
    // On Unix or if style is posix, do nothing.
}

std::string
files::getFileName(
    std::string_view pathName)
{
    return std::filesystem::path(pathName).filename().string();
}

Expected<std::string>
files::getFileText(
    std::string_view pathName)
{
    std::ifstream file((std::string(pathName)));
    if(! file.good())
        return Unexpected(formatError("std::ifstream(\"{}\" returned \"{}\"",
            pathName, std::error_code(errno, std::generic_category())));
    std::istreambuf_iterator<char> it(file);
    std::istreambuf_iterator<char> const end;
    std::string text(it, end);
    if(! file.good())
        return Unexpected(formatError("getFileText(\"{}\") returned \"{}\"",
            pathName, std::error_code(errno, std::generic_category())));
    return text;
}

namespace files {

std::string_view
getSourceFilename(
    std::string_view pathName)
{
    std::filesystem::path path(pathName);
    std::string_view result = pathName;
    for (auto it = path.end(); it != path.begin(); )
    {
        --it;
        if (*it == "source" || *it == "include")
        {
            ++it;
            std::filesystem::path subPath;
            for (auto jt = it; jt != path.end(); ++jt)
            {
                subPath /= *jt;
            }
            auto subStr = subPath.string();
            auto pos = pathName.find(subStr);
            if (pos != std::string_view::npos)
            {
                result = pathName.substr(pos);
            }
            break;
        }
    }

    return result;
}

}

#if 0
Expected<void>
forEachFile(
    std::string_view dirPath,
    bool recursive,
    AnyFileVisitor& visitor)
{
    namespace fs = llvm::sys::fs;
    namespace path = llvm::sys::path;

    std::error_code ec;
    fs::directory_iterator const end{};
    fs::directory_iterator it(dirPath, ec, false);
    MRDOCS_CHECK(!ec, formatError("fs::directory_iterator(\"{}\") returned \"{}\"", dirPath, ec));
    while (it != end)
    {
        if(it->type() == fs::file_type::directory_file)
        {
            auto s = it->path();
            MRDOCS_TRY(visitor.visitFile(s));
            if (recursive)
            {
                MRDOCS_TRY(forEachFile(it->path(), recursive, visitor));
            }
        }
        else if (it->type() == fs::file_type::regular_file)
        {
            MRDOCS_TRY(visitor.visitFile(it->path()));
        }
        // else, we don't handle this type
        it.increment(ec);
        MRDOCS_CHECK(!ec, formatError("directory_iterator::increment returned \"{}\"", ec));
    }
    return {};
}

//------------------------------------------------
//
// files
//
//------------------------------------------------

namespace files {

Expected<FileType>
getFileType(
    std::string_view pathName)
{
    namespace fs = llvm::sys::fs;
    fs::file_status fileStatus;
    if(auto ec = fs::status(pathName, fileStatus))
    {
        if(ec == std::errc::no_such_file_or_directory)
            return FileType::not_found;
        return Unexpected(Error(ec));
    }
    switch(fileStatus.type())
    {
    case fs::file_type::regular_file:
        return FileType::regular;

    case fs::file_type::directory_file:
        return FileType::directory;

    case fs::file_type::symlink_file:
    case fs::file_type::block_file:
    case fs::file_type::character_file:
    case fs::file_type::fifo_file:
    case fs::file_type::socket_file:
    case fs::file_type::type_unknown:
        return FileType::other;

    case fs::file_type::file_not_found:
    case fs::file_type::status_error:
    default:
        MRDOCS_UNREACHABLE();
    }
}

bool
isAbsolute(
    std::string_view pathName) noexcept
{
    namespace path = llvm::sys::path;

    return path::is_absolute(pathName);
}

Expected<void>
requireAbsolute(
    std::string_view pathName)
{
    MRDOCS_CHECK(isAbsolute(pathName), formatError("\"{}\" is not an absolute path"));
    return {};
}

bool
isDirsy(
    std::string_view pathName) noexcept
{
    namespace path = llvm::sys::path;

    if(pathName.empty())
        return false;
    if(! path::is_separator(
           pathName.back(),
           path::Style::native))
        return false;
    return true;
}

std::string
normalizePath(
    std::string_view pathName)
{
    namespace path = llvm::sys::path;
    SmallPathString result(pathName);
    path::remove_dots(result, true);
    return static_cast<std::string>(result.str());
}

std::string
normalizeDir(
    std::string_view pathName)
{
    return normalizePath(pathName);
}

std::string
getParentDir(
    std::string_view pathName)
{
    return llvm::sys::path::parent_path(pathName).str();
}

std::string
getParentDir(
    std::string_view const pathName,
    unsigned const levels)
{
    std::string res(pathName);
    for (unsigned i = 0; i < levels; ++i)
    {
        res = getParentDir(res);
    }
    return res;
}

std::string
makeDirsy(
    std::string_view pathName)
{
    namespace path = llvm::sys::path;

    std::string result = static_cast<std::string>(pathName);
    if (!result.empty())
    {
        const char c = result.back();
        if (!path::is_separator(c, path::Style::windows_slash))
        {
            auto const sep = path::get_separator(path::Style::native);
            result.push_back(sep.front());
        }
    }
    return result;
}

Expected<std::string>
makeAbsolute(
    std::string_view pathName)
{
    namespace fs = llvm::sys::fs;
    namespace path = llvm::sys::path;

    SmallPathString result(pathName);
    if(auto ec = fs::make_absolute(result))
        return Unexpected(formatError("fs::make_absolute(\"{}\") returned \"{}\"", pathName, ec));
    return static_cast<std::string>(result);
}

std::string
makeAbsolute(
    std::string_view pathName,
    std::string_view workingDir)
{
    namespace path = llvm::sys::path;

    if(! path::is_absolute(pathName))
    {
        SmallPathString result(workingDir);
        path::append(result, path::Style::native, pathName);
        path::remove_dots(result, true);//, path::Style::native);
        return std::string(result);
    }

    SmallPathString result(pathName);
    path::remove_dots(result, true, path::Style::native);
    return std::string(result);
}

std::string
makePosixStyle(std::string_view pathName)
{
    SmallPathString result(pathName);
    llvm::sys::path::native(result, llvm::sys::path::Style::posix);
    return std::string(result);
}

bool
isPosixStyle(std::string_view pathName)
{
    namespace path = llvm::sys::path;

    if(pathName.empty())
    {
        return true;
    }
    llvm::StringRef separator = llvm::sys::path::get_separator(path::Style::windows);
    if (pathName.find(separator) != llvm::StringRef::npos)
    {
        return false;
    }
    return true;
}


std::string
withExtension(
    std::string_view fileName,
    std::string_view ext)
{
    namespace path = llvm::sys::path;

    SmallPathString temp(fileName);
    path::replace_extension(
        temp, ext, path::Style::windows_slash);
    return std::string(temp);
}

std::string
appendPath(
    std::string_view basePath,
    std::string_view name)
{
    namespace path = llvm::sys::path;
    SmallPathString temp(basePath);
    path::append(temp, name);
    path::remove_dots(temp, true);
    return static_cast<std::string>(temp.str());
}

std::string
appendPath(
    std::string_view basePath,
    std::string_view name1,
    std::string_view name2)
{
    namespace path = llvm::sys::path;

    SmallPathString temp(basePath);
    path::append(temp, name1, name2);
    path::remove_dots(temp, true);
    return static_cast<std::string>(temp.str());
}

std::string
appendPath(
    std::string_view basePath,
    std::string_view name1,
    std::string_view name2,
    std::string_view name3)
{
    namespace path = llvm::sys::path;

    SmallPathString temp(basePath);
    path::append(temp, name1, name2, name3);
    path::remove_dots(temp, true);
    return static_cast<std::string>(temp.str());
}

std::string
appendPath(
    std::string_view basePath,
    std::string_view name1,
    std::string_view name2,
    std::string_view name3,
    std::string_view name4)
{
    namespace path = llvm::sys::path;

    SmallPathString temp(basePath);
    path::append(temp, name1, name2, name3, name4);
    path::remove_dots(temp, true);
    return static_cast<std::string>(temp.str());
}

Expected<void>
requireDirectory(
    std::string_view pathName)
{
    namespace fs = llvm::sys::fs;

    fs::file_status fileStatus;
    auto ec = fs::status(pathName, fileStatus);
    MRDOCS_CHECK(!ec, formatError(R"(fs::status("{}") returned "{}")", pathName, ec));
    MRDOCS_CHECK(fileStatus.type() == fs::file_type::directory_file, formatError("\"{}\" is not a directory", pathName));
    return {};
}

bool
isDirectory(
    std::string_view pathName)
{
    namespace fs = llvm::sys::fs;
    fs::file_status fileStatus;
    if (auto ec = fs::status(pathName, fileStatus))
    {
        return false;
    }
    return fileStatus.type() == fs::file_type::directory_file;
}

bool
isLexicalDirectory(
    std::string_view pathName)
{
    namespace fs = llvm::sys::fs;
    fs::file_status fileStatus;
    if (auto const ec = fs::status(pathName, fileStatus);
        ec ||
        fileStatus.type() == fs::file_type::file_not_found)
    {
        auto const filename = getFileName(pathName);
        return !contains(filename, '.');
    }
    return fileStatus.type() == fs::file_type::directory_file;
}

bool
exists(
    std::string_view pathName)
{
    namespace fs = llvm::sys::fs;
    fs::file_status fileStatus;
    if(auto ec = fs::status(pathName, fileStatus))
        return false;
    return exists(fileStatus);
}

Expected<void>
createDirectory(
    std::string_view pathName)
{
    namespace fs = llvm::sys::fs;

    auto kind = files::getFileType(pathName);
    MRDOCS_CHECK(kind, kind.error());
    MRDOCS_CHECK_OR(*kind != files::FileType::directory, {});
    MRDOCS_CHECK(
        *kind == files::FileType::not_found,
        formatError("creating the directory \"{}\""
                    " would overwrite an existing file", pathName));
    auto ec = fs::create_directories(pathName);
    MRDOCS_CHECK(!ec, formatError("fs::create_directories(\"{}\") returned \"{}\"", pathName, ec));
    return {};
}

bool
startsWith(
    std::string_view pathName,
    std::string_view prefix)
{
    auto itPath = pathName.begin();
    auto itPrefix = prefix.begin();
    while (itPath != pathName.end() && itPrefix != prefix.end()) {
        if (*itPath != *itPrefix) {
            char pathChar = (*itPath == '\\') ? '/' : *itPath;
            char prefixChar = (*itPrefix == '\\') ? '/' : *itPrefix;
            if (pathChar != prefixChar)
            {
                return false;
            }
        }
        ++itPath;
        ++itPrefix;
    }
    // Have we consumed the whole prefix?
    return itPrefix == prefix.end() && (itPath == pathName.end() || *itPath == '/' || *itPath == '\\');
}

} // files

ScopedTempFile::
~ScopedTempFile()
{
    if (ok_)
    {
        llvm::sys::fs::remove(path_);
    }
}

ScopedTempFile::
ScopedTempFile(
    llvm::StringRef prefix,
    llvm::StringRef ext)
{
    llvm::SmallString<128> tempPath;
    ok_ = !llvm::sys::fs::createTemporaryFile(prefix, ext, tempPath);
    if (ok_)
    {
        path_ = tempPath;
    }
}

ScopedTempDirectory::
~ScopedTempDirectory() {
    if (*this)
    {
        llvm::sys::fs::remove_directories(path_);
    }
}

ScopedTempDirectory::
ScopedTempDirectory(
    llvm::StringRef prefix)
{
    llvm::SmallString<128> tempPath;
    if (llvm::sys::fs::createUniqueDirectory(prefix, tempPath))
    {
        status_ = ErrorStatus::CannotCreateDirectories;
        return;
    }
    path_ = tempPath;
}

ScopedTempDirectory::
ScopedTempDirectory(
    llvm::StringRef root,
    llvm::StringRef dir)
{
    llvm::SmallString<128> tempPath(root);
    llvm::sys::path::append(tempPath, dir);
    bool const exists = llvm::sys::fs::exists(tempPath);
    if (exists &&
        llvm::sys::fs::remove_directories(tempPath))
    {
        status_ = ErrorStatus::CannotDeleteExisting;
        return;
    }
    if (llvm::sys::fs::create_directories(tempPath))
    {
        status_ = ErrorStatus::CannotCreateDirectories;
        return;
    }
    path_ = tempPath;
}

Error
ScopedTempDirectory::
error() const
{
    if (status_ == ErrorStatus::CannotDeleteExisting)
    {
        return Error("Failed to delete existing directory");
    }
    if (status_ == ErrorStatus::CannotCreateDirectories)
    {
        return Error("Failed to create directories");
    }
    return Error();
}
#endif
}
