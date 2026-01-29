#!/bin/sh

echo ""
echo "Use: $0 ./program"
echo ""

EXIT_CODE=1
while [ $EXIT_CODE -gt 0 ]; do
    echo "Executing $1 ..."
    $1
    EXIT_CODE=$?
    echo "finished with exit code $EXIT_CODE"
done