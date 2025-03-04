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

#include "googlecpp/g1163/libtooling/checker.h"

#include <glog/logging.h>

#include <string>

#include "absl/strings/str_format.h"
#include "misra/libtooling_utils/libtooling_utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using analyzer::proto::ResultsList;

namespace {

void ReportError(string path, int line_number, ResultsList* results_list) {
  string error_message = "Do not use a class simply to group static members";
  misra::proto_util::AddResultToResultsList(results_list, path, line_number,
                                            error_message);
  LOG(INFO) << absl::StrFormat("%s, path: %s, line: %d", error_message, path,
                               line_number);
}

}  // namespace

namespace googlecpp {
namespace g1163 {
namespace libtooling {

class Callback : public MatchFinder::MatchCallback {
 public:
  void Init(ResultsList* results_list, MatchFinder* finder) {
    results_list_ = results_list;
    // Use isDefinition() to filter out class without body.
    // Use isClass() to filter out struct, union and enum.
    finder->addMatcher(cxxRecordDecl(isDefinition(), isClass()).bind("_class"),
                       this);
  }

  void run(const MatchFinder::MatchResult& result) override {
    if (const auto* classRecord =
            result.Nodes.getNodeAs<CXXRecordDecl>("_class")) {
      if (misra::libtooling_utils::IsInSystemHeader(classRecord,
                                                    result.Context))
        return;

      // Implicitly generated class is generated by compilers
      // It is not written explicitly in the source code, so just skip
      if (classRecord->isImplicit()) return;

      // Report error only if all methods are static
      for (auto* method : classRecord->methods()) {
        if (!method->isStatic()) {
          return;
        }
      }

      // If it has some field decl,
      // the class is not just a group of static members
      // Notice that "static int x;" in a class is VarDecl but not FieldDecl
      for (auto* field : classRecord->fields()) {
        return;
      }

      ReportError(
          misra::libtooling_utils::GetFilename(classRecord,
                                               result.SourceManager),
          misra::libtooling_utils::GetLine(classRecord, result.SourceManager),
          results_list_);
    }
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
}  // namespace g1163
}  // namespace googlecpp
