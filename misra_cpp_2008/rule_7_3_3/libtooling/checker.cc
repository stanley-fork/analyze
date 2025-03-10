/*
NaiveSystems Analyze - A tool for static code analysis
Copyright (C) 2023  Naive Systems Ltd.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "misra_cpp_2008/rule_7_3_3/libtooling/checker.h"

#include <glog/logging.h>

#include <string>

#include "absl/strings/str_format.h"
#include "misra/libtooling_utils/libtooling_utils.h"

using std::string;
using namespace clang;
using namespace clang::ast_matchers;
using namespace misra::proto_util;
using analyzer::proto::ResultsList;

namespace {

void ReportError(string path, int line_number, ResultsList* results_list) {
  string error_message = "头文件中不应存在未命名的命名空间";
  analyzer::proto::Result* pb_result =
      AddResultToResultsList(results_list, path, line_number, error_message);
  pb_result->set_error_kind(
      analyzer::proto::Result_ErrorKind_MISRA_CPP_2008_RULE_7_3_3);
}

}  // namespace

namespace misra_cpp_2008 {
namespace rule_7_3_3 {
namespace libtooling {

class NamespaceCallback : public MatchFinder::MatchCallback {
 public:
  void Init(MatchFinder* finder, ResultsList* results_list) {
    results_list_ = results_list;
    finder->addMatcher(namespaceDecl(isAnonymous()).bind("namespace"), this);
  }

  void run(const MatchFinder::MatchResult& result) override {
    const NamespaceDecl* namespace_ =
        result.Nodes.getNodeAs<NamespaceDecl>("namespace");
    string path_ =
        misra::libtooling_utils::GetFilename(namespace_, result.SourceManager);
    int line_number_ =
        misra::libtooling_utils::GetLine(namespace_, result.SourceManager);
    const SourceLocation loc = namespace_->getLocation();
    if (!result.Context->getSourceManager().isInSystemHeader(loc) &&
        misra::libtooling_utils::isInHeader(namespace_, result.SourceManager)) {
      ReportError(path_, line_number_, results_list_);
    }
  }

 private:
  ResultsList* results_list_;
};

void Checker::Init(ResultsList* results_list) {
  results_list_ = results_list;
  callback_ = new NamespaceCallback;
  callback_->Init(&finder_, results_list);
}

}  // namespace libtooling
}  // namespace rule_7_3_3
}  // namespace misra_cpp_2008
