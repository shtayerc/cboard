#!/bin/sh
url="https://explorer.lichess.ovh/masters"
while read input
do
    command="$(echo "$input" | awk '{print $1}')"
    case "$command" in
        "fen")
            fen="$(echo "$input" | cut -d' ' -f2-)"
            response="$(curl -sG --data-urlencode "fen=$fen" "$url")"
            opening="$(echo "$response" | jq -rcM '.opening | "\(.eco) \(.name)"')\n"
            [ "$opening" = "null null\n" ] && opening=""
            moves="$(echo "$response" | jq -rcM '.moves[] | "\(.san) (\(.white+.black+.draws)) [\((.white/(.white+.draws+.black)*100) | round)% | \((.draws/(.white+.draws+.black)*100) | round)% | \((.black/(.white+.draws+.black))*100 | round)%]"')"
            moves="${opening}${moves}"
            response="${moves:-No explorer data}"
            echo "$response"
            ;;

        *)
            echo "Unknown command"
            ;;
    esac
done
