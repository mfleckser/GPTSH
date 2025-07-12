echo "Making request"
curl "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent" \
  -H "x-goog-api-key: $GEMINI_API_KEY" \
  -H 'Content-Type: application/json' \
  -d "{
    \"system_instruction\": {
      \"parts\": [
        {
          \"text\": \"$1\"
        }
      ]
    },
    "generationConfig": {
      "maxOutputTokens": 480
    },
    \"contents\": [
      {
        \"parts\": [
          {
            \"text\": \"$2\"
          }
        ]
      }
    ]
  }"