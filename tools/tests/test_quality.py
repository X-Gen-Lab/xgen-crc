"""Regression tests for quality-gate input and result validation."""

import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))
import quality


class QualityTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        (self.root / "tools").mkdir()
        (self.root / "tools/quality.json").write_bytes(
            (TOOLS / "quality.json").read_bytes()
        )
        self.runner = quality.Runner(self.root)

    def write(self, name, content):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8", newline="")
        return path

    def test_rejects_path_outside_repository(self):
        with self.assertRaisesRegex(quality.QualityError, "outside"):
            self.runner.select_files(["../escape.c"])

    def test_rejects_explicit_excluded_source(self):
        self.write("out/generated.c", "int value;\n")
        with self.assertRaisesRegex(quality.QualityError, "scope"):
            self.runner.select_files(["out/generated.c"])

    def test_unknown_path_is_not_treated_as_deleted(self):
        with mock.patch.object(self.runner, "git_files", return_value=set()):
            with self.assertRaisesRegex(quality.QualityError, "unknown"):
                self.runner.select_files(["src/missing.c"])

    def test_known_deleted_path_is_ignored(self):
        with mock.patch.object(
            self.runner, "git_files", return_value={"src/removed.c"}
        ):
            self.assertEqual(self.runner.select_files(["src/removed.c"]), [])

    def test_file_with_spaces_is_one_input(self):
        path = self.write("docs/name with spaces.md", "Valid text.\n")
        self.assertEqual(self.runner.select_files([str(path)]), [path])

    def test_text_rejects_crlf(self):
        path = self.write("src/example.c", "int value;\r\n")
        with self.assertRaisesRegex(quality.QualityError, "LF"):
            self.runner.check_text([path])

    def test_markdown_allows_exact_two_space_line_break(self):
        path = self.write("docs/example.md", "First line.  \nSecond.\n")
        self.runner.check_text([path])

    def test_test_discovery_rejects_empty_suite(self):
        with self.assertRaisesRegex(quality.QualityError, "no tests"):
            quality.selected_tests({"tests": []})

    def test_test_discovery_rejects_disabled_case(self):
        document = {"tests": [{"name": "Example.Case", "command": ["test"],
                                "properties": [{"name": "DISABLED", "value": True}]}]}
        with self.assertRaisesRegex(quality.QualityError, "disabled"):
            quality.selected_tests(document)

    def test_junit_rejects_all_skipped(self):
        path = self.write("result.xml", '<testsuite><testcase name="A" '
                          'status="notrun"><skipped/></testcase></testsuite>')
        with self.assertRaisesRegex(quality.QualityError, "skip|notrun"):
            quality.validate_junit(path, ["A"])

    def test_junit_rejects_missing_selected_name(self):
        path = self.write("result.xml", '<testsuite><testcase name="A" '
                          'status="run"/></testsuite>')
        with self.assertRaisesRegex(quality.QualityError, "names"):
            quality.validate_junit(path, ["A", "B"])

    def test_junit_accepts_exact_executed_suite(self):
        path = self.write("result.xml", '<testsuite><testcase name="A" '
                          'status="run"/><testcase name="B" '
                          'status="run"/></testsuite>')
        quality.validate_junit(path, ["A", "B"])

    def test_nonzero_tool_status_propagates(self):
        result = subprocess.CompletedProcess(["analyzer"], 7, "", "parse failed")
        with mock.patch("quality.subprocess.run", return_value=result):
            with self.assertRaisesRegex(quality.QualityError, "7.*parse failed"):
                self.runner.run(["analyzer"])

    def test_wrong_tool_version_is_rejected(self):
        with mock.patch.object(self.runner, "run", return_value="clang-format version 18.1.0"):
            with self.assertRaisesRegex(quality.QualityError, "19.1.5"):
                self.runner.tool("clang_format")


if __name__ == "__main__":
    unittest.main()
