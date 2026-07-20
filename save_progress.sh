#!/bin/bash

# Check if the user provided a valid commit message
if [ "$#" -ne 1 ]; then
	echo "Ussage: ./save_progress.sh \"commit message\""
	exit 1
fi

# This takes 
MESSAGE="$1"

echo "Running the script..."

git add .
git commit -m "$MESSAGE"
git push

echo "Script completed successfully"
