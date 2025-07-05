#!/bin/bash

# Helper script to run a Lambda (.lba) file using the Lambda interpreter
# from the separate Lambda language repository.

# --- Configuration ---
# The absolute path to your Lambda language repository.
# IMPORTANT: Update this path if your Lambda project is located elsewhere.
LAMBDA_PROJECT_PATH="/Users/dylando/Lambda"
# --- End Configuration ---

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check if a file path was provided as an argument
if [ -z "$1" ]; then
    echo -e "${RED}[!] Error: You must provide the path to a .lba file.${NC}"
    echo "Usage:  ./tools/run_lambda.sh <path/to/your/script.lba>"
    exit 1
fi

LAMBDA_SCRIPT_ARG=$1
INTERPRETER_PATH="$LAMBDA_PROJECT_PATH/src/main.js"

# Check if the interpreter exists
if [ ! -f "$INTERPRETER_PATH" ]; then
    echo -e "${RED}[!] Error: Lambda interpreter not found at '$INTERPRETER_PATH'.${NC}"
    echo -e "${YELLOW}Please check the LAMBDA_PROJECT_PATH in this script.${NC}"
    exit 1
fi

# Check if the script file to run exists
if [ ! -f "$LAMBDA_SCRIPT_ARG" ]; then
    echo -e "${RED}[!] Error: Lambda script not found at '$LAMBDA_SCRIPT_ARG'.${NC}"
    echo -e "${YELLOW}Please ensure the path is correct relative to your current directory: $(pwd)${NC}"
    exit 1
fi

# Resolve the absolute path for the script to ensure the interpreter can find it.
if ! command -v realpath &> /dev/null; then
    echo -e "${YELLOW}[!] Warning: 'realpath' command not found. Using a fallback method for path resolution.${NC}"
    ABSOLUTE_SCRIPT_PATH="$(cd "$(dirname "$LAMBDA_SCRIPT_ARG")" && pwd)/$(basename "$LAMBDA_SCRIPT_ARG")"
else
    ABSOLUTE_SCRIPT_PATH=$(realpath "$LAMBDA_SCRIPT_ARG")
fi

echo -e "${GREEN}[*] Running Lambda script: $ABSOLUTE_SCRIPT_PATH${NC}"
echo "---"

# Execute the interpreter with the provided script
node "$INTERPRETER_PATH" "$ABSOLUTE_SCRIPT_PATH"