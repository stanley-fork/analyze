cc_library(
    name = "rule_1_lib",
    srcs = ["checker.cc"],
    visibility = ["//visibility:public"],
    deps = ["//sdk/checker"],
    alwayslink = True,
)

cc_binary(
    name = "rule_1",
    deps = [
        ":rule_1_lib",
        "//podman_image/bigmain",
    ],
)
