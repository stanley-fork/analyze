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

package find_alloca

import (
	"path/filepath"
	"testing"

	"naive.systems/analyzer/cruleslib/runner"
	"naive.systems/analyzer/cruleslib/testlib"
	"naive.systems/analyzer/sdk/testcase"
)

func TestGood0001(t *testing.T) {
	tc := testcase.NewWithoutOptions(t, "_good0001")
	tc.ExpectOK(testlib.ToTestResult(runner.RunCoccinelle(tc.Srcdir, filepath.Dir(tc.Srcdir))))
}

func TestBad0001(t *testing.T) {
	tc := testcase.NewWithoutOptions(t, "_bad0001")
	tc.ExpectOK(testlib.ToTestResult(runner.RunCoccinelle(tc.Srcdir, filepath.Dir(tc.Srcdir))))
}
