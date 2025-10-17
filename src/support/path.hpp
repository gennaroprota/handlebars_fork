//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2025 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/handlebars

#ifndef BOOST_HANDLEBARS_SRC_SUPPORT_PATH_HPP
#define BOOST_HANDLEBARS_SRC_SUPPORT_PATH_HPP

#include <boost/handlebars/config.hpp>
#include <boost/handlebars/support/path.hpp>
#include <filesystem>
#include <string>

namespace boost::handlebars {

using PathString = std::string;

enum class PathStyle
{
    native,
    posix,
    windows
};

/** Replaces backslashes with slashes if Windows in place.

    @param path A path that is transformed to native format.

    On Unix, this function is a no-op because backslashes
    are valid path chracters.
*/
void
convert_to_slash(
    std::filesystem::path & path,
    PathStyle style = PathStyle::native);

#if 0
/** A temporary file that is deleted when it goes out of scope.
*/
class ScopedTempFile
{
    PathString path_;
    bool ok_ = false;
public:
    /** Destructor

        If a file was created, it is deleted.

     */
    ~ScopedTempFile();

    /** Constructor

        Creates a temporary file with the given prefix and extension.
        The file is deleted when this object goes out of scope.

        The file is created in the system temporary directory with
        the following format:

        @code
        <tempdir>/<prefix><random>.<ext>
        @endcode

        @param prefix The prefix for the temporary file name.
        @param ext The extension for the temporary file name.
    */
    ScopedTempFile(std::string_view prefix, std::string_view ext);

    /** Returns `true` if the file was created successfully.
    */
    operator bool() const { return ok_; }

    /** Returns the path to the temporary file.
     */
    std::string_view path() const { return path_; }
};

/** A temporary directory that is deleted when it goes out of scope.
*/
class ScopedTempDirectory
{
    // Status of the directory
    enum class ErrorStatus
    {
        None,
        CannotDeleteExisting,
        CannotCreateDirectories
    };

    PathString path_;
    ErrorStatus status_ = ErrorStatus::None;
public:
    /** Destructor

        If a directory was created, it is deleted.

     */
    ~ScopedTempDirectory();

    /** Constructor

        Creates a temporary directory with the given prefix.
        The directory is deleted when this object goes out of scope.

        The directory is created in the system temporary directory with
        the following format:

        @code
        <tempdir>/<prefix><random>
        @endcode

        For instance, if the prefix is "mrdocs" and the operating system
        is Unix, the directory might be created as: "/tmp/mrdocs-1234".

        On Windows, the directory might be created as:
        "C:\Users\user\AppData\Local\Temp\mrdocs-1234".

        @param prefix The prefix for the temporary directory name.
    */
    ScopedTempDirectory(llvm::StringRef prefix);

    /** Constructor with a specific path

        Creates a temporary directory with the given path.
        The directory is deleted when this object goes out of scope.

        @param root The root directory for the temporary directory.
        @param dir The name of the temporary directory.
    */
    ScopedTempDirectory(llvm::StringRef root, llvm::StringRef dir);

    /** Returns `true` if the directory was created successfully.
    */
    operator bool() const
    {
        return status_ == ErrorStatus::None;
    }

    /** Returns `true` if the directory was not created successfully.
    */
    bool
    failed() const
    {
        return status_ != ErrorStatus::None;
    }

    /** Returns the path to the temporary directory.
     */
    std::string_view path() const { return static_cast<llvm::StringRef>(path_); }

    /** Returns the error status of the directory.
     */
    Error error() const;

    /** Convert temp directory to a std::string_view
     */
    operator std::string_view() const { return path(); }
};
#endif

}

#endif
