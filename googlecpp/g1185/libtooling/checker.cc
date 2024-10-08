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

#include "googlecpp/g1185/libtooling/checker.h"

#include <glog/logging.h>

#include <string>

#include "absl/strings/str_format.h"
#include "misra/libtooling_utils/libtooling_utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using analyzer::proto::ResultsList;

namespace {

void ReportError(string path, int line_number, ResultsList* results_list) {
  string error_message =
      "Do not put large method definitions inline in the class definition";
  misra::proto_util::AddResultToResultsList(results_list, path, line_number,
                                            error_message);
  LOG(INFO) << absl::StrFormat("%s, path: %s, line: %d", error_message, path,
                               line_number);
}

}  // namespace

namespace googlecpp {
namespace g1185 {
namespace libtooling {

class Callback : public MatchFinder::MatchCallback {
 public:
  void Init(ResultsList* results_list, MatchFinder* finder) {
    results_list_ = results_list;
    finder->addMatcher(cxxMethodDecl(unless(isExpansionInSystemHeader()),
                                     anyOf(isInline(), isConstexpr()))
                           .bind("func"),
                       this);
  }

  void run(const MatchFinder::MatchResult& result) override {
    const auto* func = result.Nodes.getNodeAs<FunctionDecl>("func");
    // Get the number of lines of a function
    auto* SM = result.SourceManager;
    SourceRange funcRange = func->getSourceRange();
    unsigned funcBeginLine = SM->getSpellingLineNumber(funcRange.getBegin());
    unsigned funcEndLine = SM->getSpellingLineNumber(funcRange.getEnd());
    unsigned funcLineCount = funcEndLine - funcBeginLine + 1;
    if (funcLineCount <= 10) return;
    ReportError(misra::libtooling_utils::GetFilename(func, SM),
                misra::libtooling_utils::GetLine(func, SM), results_list_);
  }

 private:
  ResultsList* results_list_;
};

void Checker::Init(ResultsList* results_list) {
  results_list_ = results_list;
  callback_ = new Callback;
  callback_->Init(results_list, &finder_);
}
}  // namespace libtooling
}  // namespace g1185
}  // namespace googlecpp
