#!/bin/sh
if cmp $2 $3; then
        echo "✅   OK $1 🎉"
else
        echo "❌   NG $1 😱";
fi
