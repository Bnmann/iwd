#ifndef IWD__SCHEMA_HPP_
#define IWD__SCHEMA_HPP_

namespace iwd {
constexpr auto kSchema = R"__json__({
    "definitions": {},
    "$schema": "http://json-schema.org/draft-04/schema#",
    "$id": "iwd",
    "type": "object",
    "title": "IWDSpecification",
    "properties": {
        "requirements": {
            "type": "array",
            "description": "List of requirements",
            "items": {
                "type": "object",
                "properties": {
                    "name": {
                        "type": "string",
                        "description": "Name of the library that will be used"
                    },
                    "version": {
                        "type": "string",
                        "description": "Target version of the library that should be used.\nWhen url points to git repository, the version should match the release tag of the repository"
                    },
                    "url": {
                        "type": "string",
                        "description": "Target url that points to either git repository or\ntar gz file with the sources"
                    },
                    "build": {
                        "type": "object",
                        "cmake_build": {
                            "type": "boolean",
                            "description": "Controls whether the project should be build and installed with cmake. Defaults to true.\nSet it to false when some library does not provide CMakelists.txt or is header-only.",
                            "default": true
                        },
                        "cmake_directory": {
                            "type": "string",
                            "description": "Some of the projects have CMakeLists.txt inside non project-root directory. If that's the case provide relative path to the project root that points to the directory containing CMakeLists",
                            "default": "null"
                        }
                    },
                    "configuration": {
                        "type": "object",
                        "description": "Dictionary of key value pairs, where keys are\ncmake variable names.\nThose values strictly correspond to the -D{key}={value} args provided in the command line when configuring with cmake"
                    },
                    "copy": {
                        "type": "array",
                        "description": "After sourcing the file, provide a list of copy objects\nthat describe what to copy to the install directory",
                        "items": {
                            "type": "object",
                            "properties": {
                                "keep-paths": {
                                    "type": "boolean",
                                    "description": "Controls whether to keep paths when copying found files",
                                    "default": true
                                },
                                "source-directory": {
                                    "type": "string",
                                    "description": "Provide a path to the directory, which will be treated as a root directory for any expression and path resolving.\nIf the value is not provided, if defaults to the dependency source directory."
                                },
                                "sources": {
                                    "type": "string",
                                    "description": "python glob expression , controls what kind of files should be copied"
                                },
                                "destination": {
                                    "type": "string",
                                    "description": "Relative path to the iwd install directory, defines where are the found files copied"
                                }
                            },
                            "required": [
                                "sources",
                                "destination"
                            ]
                        }
                    },
                    "patch": {
                        "type": "array",
                        "items": {
                            "type": "object",
                            "properties": {
                                "type": {
                                    "type": "string",
                                    "enum": [
                                        "replace",
                                        "append",
                                        "create",
                                        "apply"
                                    ],
                                    "description": "Describes what kind of patch command is this"
                                },
                                "file": {
                                    "type": "string",
                                    "description": "Describes the relative path from source directory to the file on which the operation should be executed."
                                },
                                "text": {
                                    "type": "array",
                                    "items": {
                                        "type": "string"
                                    },
                                    "description": "Lines, which will be added to the file"
                                },
                                "pattern": {
                                    "type": "string",
                                    "description": "Required only for replace command. Regex pattern that will serve as a target for replacement."
                                }
                            },
                            "required": [
                                "type",
                                "file"
                            ]
                        }
                    }
                },
                "required": [
                    "name",
                    "version",
                    "url"
                ]
            }
        },
        "iwd_version": {
            "type": "string",
            "description": "Minimal iwd version required"
        }
    },
    "required": [
        "requirements",
        "iwd_version"
    ]
}

)__json__";
}

#endif
