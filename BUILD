package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "stonks",
    data = ["//content"],
    srcs = ["main.cc"],
    deps = [
        "@libgam//:game",
        ":screens",
    ],
)

cc_binary(
    name = "server",
    srcs = ["server.cc"],
    deps = [
        "@libgam//:server",
        "@libgam//:util",
    ],
)

cc_library(
    name = "screens",
    srcs = [
        "title_screen.cc",
    ],
    hdrs = [
        "title_screen.h",
    ],
    deps = [
        "@libgam//:screen",
        "@libgam//:text",
    ],
)
