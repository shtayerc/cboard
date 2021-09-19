#!/bin/sh
url="https://explorer.lichess.ovh/master"
while read input
do
    command="$(echo "$input" | awk '{print $1}')"
    case "$command" in
        "fen")
            fen="$(echo "$input" | cut -d' ' -f2-)"
            response="$(curl -sX 'GET' --data-urlencode "fen=$fen" "$url" | jq -rcM '.moves[] | "\(.san) (\(.white+.black+.draws)) [\((.white/(.white+.draws+.black)*100) | round)% | \((.draws/(.white+.draws+.black)*100) | round)% | \((.black/(.white+.draws+.black))*100 | round)%]"')"
            echo "$response"
            ;;

        *)
            echo "Unknown command"
            ;;
    esac
done
