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

#include <glog/logging.h>

#include "absl/strings/str_format.h"
#include "libtooling_includes/cmd_options.h"
#include "podman_image/bigmain/rule.h"

using Rule = podman_image::bigmain::Rule;

int main(int argc, char** argv) {
  std::vector<Rule*>& all_rules = Rule::GetAllRules();
  if (all_rules.size() == 1) {
    return all_rules[0]->Run(argc, argv);
  }
  for (Rule* rule : all_rules) {
    int r;
    if (rule->Entrypoint(argc, argv, &r)) return r;
  }
  google::InitGoogleLogging(argv[0]);
  LOG(ERROR) << absl::StrFormat("rules size: %d, incompatible argv[0]: %s",
                                all_rules.size(), argv[0]);
  return -1;
}
