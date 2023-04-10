#!/bin/sh
url="https://explorer.lichess.ovh/masters"
mode=0

mode_handle() {
    case "$mode" in
        0)
            moves_get "$response"
            ;;
        1)
            games_get "$response"
            ;;
    esac
}

response_query() {
    echo "$response" | jq -rcM "$1"
}

games_get() {
    games="$(response_query '.topGames[].winner |= . + "1/2-1/2" | .topGames[] | .winner |= sub("white1/2-1/2"; "1-0") | .winner |= sub("black1/2-1/2"; "0-1") | "\(.white.name)-\(.black.name)/\(.year) (\(.winner))"')"
    echo "${games:-No games found}"
}

moves_get() {
    opening="$(response_query '.opening | "\(.eco) \(.name)"')\n"
    [ "$opening" = "null null\n" ] && opening=""
    moves="$(response_query '.moves[] | "\(.san) (\(.white+.black+.draws)) [\((.white/(.white+.draws+.black)*100) | round)% | \((.draws/(.white+.draws+.black)*100) | round)% | \((.black/(.white+.draws+.black))*100 | round)%]"')"
    moves="${opening}${moves}"
    echo "${moves:-No moves found}"
}

while read -r input
do
    command="$(echo "$input" | awk '{print $1}')"
    case "$command" in
        "fen")
            fen="$(echo "$input" | cut -d' ' -f2-)"
            response="$(curl -sG --data-urlencode "fen=$fen" --data-urlencode "topGames=9" "$url")"
            mode_handle
            ;;
        "event")
            event="$(echo "$input" | awk '{print $2}')"
            case "$event" in
                0)
                    mode=$((1-mode))
                    mode_handle
                    ;;
                1|2|3|4|5|6|7|8|9)
                    id="$(response_query ".topGames[$((event-1))].id")"
                    curl "$url/pgn/$id" | cboard --FEN-find "$fen" -
                    ;;
            esac
            ;;
        *)
            echo "Unknown command"
            ;;
    esac
done


