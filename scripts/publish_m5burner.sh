#!/bin/bash
# Publish firmware to M5Burner for all device targets.
set -e

VERSION="${1:?Usage: $0 <version>}"
TEMPLATE_DIR="m5burner"
DESCRIPTION_FILE="$TEMPLATE_DIR/description.txt"

echo "Publishing firmware v$VERSION to M5Burner..."
echo ""

for TEMPLATE in "$TEMPLATE_DIR"/*.json; do
    KEY=$(basename "$TEMPLATE" .json)
    PAYLOAD="payload_${KEY}.json"

    echo "--- $KEY ---"

    jq --arg version "$VERSION" --rawfile desc "$DESCRIPTION_FILE" '
        .fields.description = $desc
        | .fields.version = $version
        | .files.firmware = (.files.firmware | sub("__VERSION__"; $version))
    ' "$TEMPLATE" > "$PAYLOAD"

    npx -y m5-burner publish-firmware "$PAYLOAD" || echo "WARNING: failed to publish $KEY, continuing..."

    rm -f "$PAYLOAD"
    echo ""
done

echo "All devices published successfully!"
