#!/bin/bash

# Demo of multi-line functionality in CLL
# This script demonstrates both multi-line JavaScript and multi-line ask modes

echo "🚀 CLL Multi-line Demo"
echo "======================"
echo
echo "This demo shows:"
echo "1. Multi-line JavaScript mode (& + Enter, then Ctrl-D to execute)"
echo "2. Multi-line Ask mode (ask + Enter, then Ctrl-D to send to Claude)"
echo
echo "Press Enter to start..."
read

echo "📝 Creating demo inputs..."

# Create multi-line JS demo
cat > /tmp/js_demo.txt << 'EOF'
&
console.log('Multi-line JavaScript demo');
const nums = [1, 2, 3, 4, 5];
const sum = nums.reduce((a, b) => a + b, 0);
console.log('Sum:', sum);
Math.sqrt(64);
EOF

# Create multi-line Ask demo  
cat > /tmp/ask_demo.txt << 'EOF'
ask
Please explain the following concepts:
1. JavaScript promises
2. Async/await syntax
3. Event loop in Node.js

Please provide a brief but comprehensive explanation.
EOF

echo
echo "🔧 Demo 1: Multi-line JavaScript"
echo "Command: &"
echo "Then type multiple JS lines, press Ctrl-D to execute"
echo
echo "Input will be:"
cat /tmp/js_demo.txt
echo
echo "Running demo..."
./Bin/cll < /tmp/js_demo.txt
echo

echo "🤖 Demo 2: Multi-line Ask (would work with Claude AI if available)"
echo "Command: ask"
echo "Then type multi-line question, press Ctrl-D to send"
echo
echo "Input will be:"
cat /tmp/ask_demo.txt
echo
echo "Running demo..."
./Bin/cll < /tmp/ask_demo.txt

# Cleanup
rm -f /tmp/js_demo.txt /tmp/ask_demo.txt

echo
echo "✅ Multi-line demo complete!"
echo
echo "Summary:"
echo "- & + Enter: Start multi-line JavaScript mode"
echo "- ask + Enter: Start multi-line ask mode"  
echo "- Ctrl-D: Execute/send the multi-line input"
echo "- Regular & prefix: Execute single-line JavaScript immediately"
echo "- ask <question>: Send single-line question to Claude immediately"