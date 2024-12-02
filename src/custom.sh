#!/bin/bash

# Loop through lowercase letters a to z
for letter in {a..z}; do
  file1="output/$letter.txt"
  file2="../checker/test_out/$letter.txt"

  # Check if both files exist before running diff
  if [[ -f "$file1" && -f "$file2" ]]; then
    echo "Comparing $file1 with $file2:"
    diff_output=$(diff "$file1" "$file2")
    if [[ -n "$diff_output" ]]; then
      echo "$diff_output"
    else
      echo "No differences found."
    fi
  else
    echo "One or both files do not exist: $file1, $file2"
  fi
done
