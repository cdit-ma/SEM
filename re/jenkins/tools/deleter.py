#!/bin/python3
import os
import shutil
import subprocess
import time
import unittest
import argparse


class DeleterTest(unittest.TestCase):
    def setUp(self):
        subprocess.run("./make_tests.sh")
        self.root_dir = "python_deleter_tests"

    def test_delete_oldest(self):
        delete(self.root_dir, 5)
        found = os.listdir(self.root_dir)
        expected = ["old_file.txt", "file.txt", "old_dir", "dir"]
        intersection = list(set(found) & set(expected))
        self.assertEqual(len(expected), len(intersection))

    def test_delete_old_and_oldest(self):
        delete(self.root_dir, 2)
        found = os.listdir(self.root_dir)
        expected = ["file.txt", "dir"]
        intersection = list(set(found) & set(expected))
        self.assertEqual(len(expected), len(intersection))

    def test_delete_none(self):
        delete(self.root_dir, 100)
        found = os.listdir(self.root_dir)
        expected = ["old_file.txt", "file.txt", "oldest_file.txt", "old_dir", "dir", "oldest_dir"]
        intersection = list(set(found) & set(expected))
        self.assertEqual(len(expected), len(intersection))


def delete(root_dir, days, dry_run=False):
    now = time.time()
    for f in os.listdir(root_dir):
        try:
            if os.is_symlink(f):
                continue
            f = os.path.join(root_dir, f)
            if os.stat(f).st_mtime < now - days * 86400:
                if os.path.isfile(f):
                    if dry_run:
                        print(f)
                    else:
                        os.remove(f)
                else:
                    if dry_run:
                        print(f)
                    else:
                        shutil.rmtree(f)
        except OSError, e:
            print("Skipping ", f)


def test_suite():
    suite = unittest.TestSuite()
    suite.addTest(DeleterTest('test_delete_none'))
    suite.addTest(DeleterTest('test_delete_oldest'))
    suite.addTest(DeleterTest('test_delete_old_and_oldest'))
    return suite


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Delete all in directory older than day')
    parser.add_argument("root_dir", help="root directory to run deletion in")
    parser.add_argument("age", help="minimum age (in days) of files + directories to delete", type=int)
    parser.add_argument("--test", help="run tests",
                        action="store_true")
    parser.add_argument("--dry_run", help="run but only print list of files to be deleted", action="store_true")
    args = parser.parse_args()

    if args.test:
        runner = unittest.TextTestRunner()
        out = runner.run(test_suite())
        exit(out)

    dry_run_ = False
    if args.dry_run:
        dry_run_ = True

    delete(args.root_dir, args.age, dry_run_)
