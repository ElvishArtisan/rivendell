#!/bin/bash

## ----------------------------------------------------------------------------
#
# SCRIPT NAME:  aka_purge.sh
#
# DESCRIPTION:  Purge the Akamai cache via CCU REST API v3.
#
# REVISION:     v0.9
# DATE:         May 2019
# AUTHORS:      Luca, Genasci <luca.genasci@rsi.ch>
#               Marco, De March <mdemarch@akamai.com>
#
# CONTRIBUTORS: Raffaele, Rusconi <raffaele.rusconi@prada.com>
#
# TESTED:       Tested in:
#                   GNU bash, version 4.4.12(3)-release (x86_64-unknown-cygwin)
#                   GNU bash, version 5.0.7(1)-release (x86_64-pc-linux-gnu)
#                   GNU bash, version 4.2.46(2)-release (x86_64-redhat-linux-gnu)
#                   GNU bash, version 3.2.57(1)-release (x86_64-apple-darwin18)
#
## ----------------------------------------------------------------------------

## Definition if functions ----------------------------------------------------

function aa_set {
  local key="${1}"
  local val="${2}"
  shift 2

  declare -a arr=( ${@} )

  local t_key
  local idx=0
  for el in "${arr[@]}"
  do
    t_key="${el%%:*}"
    [ "${key}" == "${t_key}" ] && { arr[idx]="${key}:${val}"; echo "${arr[@]}"; return 0; }
    let idx++
  done

  arr[idx]="${key}:${val}"
  echo "${arr[@]}"
  return 0
}

function aa_get {
  local key="${1}"
  shift 1

  declare -a arr=( ${@} )

  for el in "${arr[@]}"
  do
    t_key="${el%%:*}"
    [ "${key}" == "${t_key}" ] && { echo "${el#*:}"; return 0; }
  done

  return 1
}

function _get_property {
  [ "$#" -lt 2 ] && return 1
  local RC=$1
  local PROP=$2
  local value=$(cat ${RC} | sed "/^\s*#/d;s/\s*#[^\"']*$//" | grep ${PROP} | tail -n 1 | cut -d '=' -f2- )
  if [ -z "${value}" ]; then
    return 1
  else
    echo ${value}
    return 0
  fi
}

function get_properties {
  local file="${1}"

  declare -a aka_props
  local tmp

  tmp=$( _get_property "${file}" client_secret )
  [ -z "${tmp}" ] && { >&2 echo "ERROR: Please, set variable client_secret in file ${file}!!!"; exit 1; }
  aka_props=( $( aa_set client_secret ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" client_token )
  [ -z "${tmp}" ] && { >&2 echo "ERROR: Please, set variable client_token in file ${file}!!!"; exit 1; }
  aka_props=( $( aa_set client_token ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" access_token )
  [ -z "${tmp}" ] && { >&2 echo "ERROR: Please, set variable access_token in file ${file}!!!"; exit 1; }
  aka_props=( $( aa_set access_token ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" host )
  [ -z "${tmp}" ] && { >&2 echo "ERROR: Please, set variable host in file ${file}!!!"; exit 1; }
  aka_props=( $( aa_set host ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" network )
  [ -z "${tmp}" ] && aka_props=( $( aa_set network staging "${aka_props[@]}" ) ) || aka_props=( $( aa_set network ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" action )
  [ -z "${tmp}" ] && aka_props=( $( aa_set action invalidate "${aka_props[@]}" ) ) || aka_props=( $( aa_set action ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" type )
  [ -z "${tmp}" ] && aka_props=( $( aa_set type url "${aka_props[@]}" ) ) || aka_props=( $( aa_set type ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" n_retries )
  [ -z "${tmp}" ] && aka_props=( $( aa_set n_retries 3 "${aka_props[@]}" ) ) || aka_props=( $( aa_set n_retries ${tmp} "${aka_props[@]}" ) )

  tmp=$( _get_property "${file}" t_retry )
  [ -z "${tmp}" ] && aka_props=( $( aa_set t_retry 60 "${aka_props[@]}" ) ) || aka_props=( $( aa_set t_retry ${tmp} "${aka_props[@]}" ) )

  echo "${aka_props[@]}"

  return 0
}

function mk_nonce {
  local s=$1
  if [ -z ${s} ]; then s=$( date -u +'%Y%m%dT%H:%M:%S%z' ); fi
  if [ $(uname) == 'Darwin' ]; then
    echo -n "${s}" | md5 -r | cut -d ' ' -f1 | sed 's/.\{4\}/&-/g' | sed 's/.$//'
  else
    echo -n "${s}" | md5sum | cut -d ' ' -f1 | sed 's/.\{4\}/&-/g' | sed 's/.$//'
  fi
}

function base64_hmac_sha256 {
  [ "$#" -lt 2 ] && return 1
  local key=$1
  local value=$2

  echo -ne "${value}"| openssl sha256 -binary -hmac "${key}" | openssl base64
}

function base64_sha256 {
   [ "$#" -lt 1 ] && return 1
   local value=$1

   echo -ne "${value}" | openssl dgst -binary -sha256 | openssl base64
}

function mk_auth_header {
  [ "$#" -lt 3 ] && return 1
  local timestamp=${1}
  local nonce=${2}
  shift 2

  declare -a aka_props=( ${@} )

  # EG1-HMAC-SHA256
  echo -n "client_token="$(aa_get client_token "${aka_props[@]}")";access_token="$( aa_get access_token "${aka_props[@]}" )";timestamp=${timestamp};nonce=${nonce};"
}

function sign_data {
  [ "$#" -lt 2 ] && return 1
  local key=${1}
  shift

  declare -a data_to_sign=( ${@} )

  local method=$( aa_get method "${data_to_sign[@]}" )
  local scheme=$( aa_get scheme "${data_to_sign[@]}" )
  local host=$( aa_get host "${data_to_sign[@]}" )
  local request_uri=$( aa_get request_uri "${data_to_sign[@]}" )
  local hash_content=$( aa_get hash_content "${data_to_sign[@]}" )
  local auth_header=$( aa_get auth_header "${data_to_sign[@]}" )

  local data="${method}\t${scheme}\t${host}\t${request_uri}\t\t${hash_content}\tEG1-HMAC-SHA256 ${auth_header}"
  base64_hmac_sha256 "${key}" "${data}"
}

function mk_body {
  local type="${1}"
  local objects="${2}"
  local domain="${3}"

  local arr_objects
  local objs

  IFS=',' read -r -a arr_objects <<< "${objects}"
  for i in ${!arr_objects[@]}
  do
    local tmp=$( echo ${arr_objects[i]} | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' -e 's,/,\\/,g' )
    if [ "${type}" == "cpcode" ]; then
      objs="${objs},${tmp}"
    else
      objs="${objs},\"${tmp}\""
    fi
  done

  objs=$( echo "${objs}" | sed 's/^,//' )

  if [ "${type}" == "url" ]; then
    echo "{\"hostname\":\"${domain}\",\"objects\":[${objs}]}"
  else
    echo "{\"objects\":[${objs}]}"
  fi
}

function get_objects {
  local file="${1}"
  local domain="${2}"
  local type="${3}"

  local objs

  case ${type} in
   url )
     cat "${file}" | grep -E '^(http(s)?://'${domain}')?/' | sed -E 's|^http(s)?://'${domain}'||' | paste -sd ','
   ;;
   cpcode )
     cat "${file}" | sed -E '/^[0-9]+$/!d' | paste -sd ','
   ;;
   tag )
     cat "${file}" | paste -sd ','
   ;;
   * ) return 1 ;;
  esac

  return 0

}

function separate_objects {
  local objs="${1}"

  local limit=49000
  local bytes buffer
  local b_len=0

  declare -a arr

  bytes=$( echo "${objs}" | wc -c )
  [ ${bytes} -le ${limit} ] && { echo ${objs}; return 0; }

  while true
  do
    buffer=${objs:${b_len}:${limit}}
    [ -z ${buffer} ] && break
    buffer=${buffer%,*}
    arr+=( ${buffer} )
    b_len=$(( ${b_len} + ${#buffer} + 1 ))
    echo ${b_len}
  done

  echo "${arr[@]}"
  return 0

}

function countdown(){
   local cn=${1}
   local lb=${2}

   date1=$((`date +%s` + ${cn}));
   while [ "$date1" -ge `date +%s` ]; do
     echo -ne "${lb}: $(date -u --date @$(($date1 - `date +%s`)) +%H:%M:%S)\r";
     sleep 0.1
   done
}

function getopt_type {
  local type="BSD"
  getopt -T > /dev/null
  [ $? -eq 4 ] && type="GNU"

  echo "${type}"
}


## Options of script ----------------------------------------------------------

P_NAME=${0##*\/}

GETOPT_TYPE=$( getopt_type )

WARNING=
[ "${GETOPT_TYPE}" != "GNU" ] && {
  WARNING+=$'\n'$'\e[93m';
  WARNING+="WARNING"$'\n';
  WARNING+="  The version of your getopt is not GNU."$'\n';
  WARNING+="  If you want use the long options use the brew command to install gnu-getopt."$'\n';
  WARNING+="  ${P_NAME} work fine with short options."$'\n';
  WARNING+=$'\e[0m';
}

CONF=/usr/share/rivendell/keys/edgegrid.conf

declare -a AKA_PROPS=$( get_properties "${CONF}" )

usage() {
cat << EOF
usage: ${P_NAME} [OPTIONS] -o <obj1[,obj2,obj3,...]>

Purge the Akamai cache via CCU REST API v3.

REMARKS
  Please create a config file in /usr/share/rivendell/keys
${WARNING}
PARAMETERS:
    -o, --objects      List of objects to purge (with comma separated values)

    -i, --input-file   Input file that contain a list of objects.
                       The separator value in the file mode is the carriage
                       return

                       Type   | Comment
                       --------------------------------------------------------
                       url    | The addresses considered, depend of the domain
                              | (see the option -d). If an address not have a
                              | domain it's included in the purge procedure.
                       --------------------------------------------------------
                       cpcode | List of the cpcodes. The items must be numbers
                              | otherwise will be excluded.
                       --------------------------------------------------------
                       tag    | List of a tags


OPTIONS:
    -t, --type         Type of objects
                          Possible values: url, cpcode or tag
                          Default: ${TYPE}

    -d, --domain       Domain site (es. your.domain.com).
                       To use with type=url

    -a, --action       The desired action to manage the cache
                          Possible values: invalidate or delete
                          Default: ${ACTION}

    -n, --network      The network on which you want to invalidate or delete content
                          Possible values: staging or production
                          Default: ${NETWORK}

    -r, --show-quests  Show the requests

    -h, --help         Show this message
    -v, --version      Show version


CONFIG FILE:

  In the config file the following values have to been declared:
    client_secret = <your client secret>
    client_token = <your client token>
    access_token = <your access token>
    host = <your akamai host>

  There is the possibility to set the default values:
    network = <staging | production>
    action  = <invalidate | delete>
    type    = <url | cpcode | tag>

  If no values are declared the default ones are:
    network = staging
    action  = invalidate
    type    = url

  For manage an error:
    n_retries = <number of retries> (default 3)
    t_retry = <time of next retry> (default 60)

EOF
}

[ "${GETOPT_TYPE}" == "GNU" ] && ARGS=$( getopt -o "d:t:o:i:a:n:rhv" -l "domain:,type:,objects:,input-file:,action:,network:,show-quests,help,version" -n "$0" -- "$@" ) || ARGS=$( getopt "d:t:o:i:a:n:rhv" "$@" )
eval set -- "$ARGS"

VERSION=0.9

NETWORK=$( aa_get network "${AKA_PROPS[@]}" )
ACTION=$( aa_get action "${AKA_PROPS[@]}" )
TYPE=$( aa_get type "${AKA_PROPS[@]}" )

N_RETRIES=$( aa_get n_retries "${AKA_PROPS[@]}" )
T_RETRY=$( aa_get t_retry "${AKA_PROPS[@]}" )

S_QUEST=0

while true; do
  case "$1" in
    -d | --domain )
      DOMAIN=${2}
      shift 2
    ;;
    -t | --type )
      TYPE=${2}
      shift 2
    ;;
    -o | --objects )
      OBJECTS=${2}
      shift 2
    ;;
    -i | --input-file )
      IFILE=${2}
      shift 2
    ;;
    -a | --action )
      ACTION=${2}
      shift 2
    ;;
    -n | --network )
      NETWORK=${2}
      shift 2
    ;;
    -r | --show-quests )
      S_QUEST=1
      shift
    ;;
    -h | --help )
      usage
      exit 0
    ;;
    -v | --version )
      echo $0 version: $VERSION
      exit 0
    ;;
    -- )
      shift
      break
    ;;
    * )
      >&2 echo "Internal error!"
      exit 1
    ;;
  esac
done

## Main -----------------------------------------------------------------------

#[ -z "${TYPE}" ] && { usage; exit 1; }
#[ -z "${OBJECTS}" ] && [ ! -f "${IFILE}" ] && { usage; exit 1; }
#[ "${TYPE}" == "url" ] && [ -z "${DOMAIN}" ] && { usage; exit 1; }

[ "${TYPE}" != "url" ] && [ ! -z "${DOMAIN}" ] && { echo "WARNING: type is different of url then -d,--domain will be ignored ..."; }

[ "${TYPE}" != "url" ] && [ "${TYPE}" != "cpcode" ] && [ "${TYPE}" != "tag" ] && { >&2 echo "ERROR: Possible value of OBJECT TYPE is url, cpcode or tag"; exit 1; }
[ "${NETWORK}" != "staging" ] && [ "${NETWORK}" != "production" ] && { >&2 echo "ERROR: Possible value of NETWORK is staging or production"; exit 1; }
[ "${ACTION}" != "invalidate" ] && [ "${ACTION}" != "delete" ] && { >&2 echo "ERROR: Possible value of ACTION is invalidate or delete"; exit 1; }

[ -z "${OBJECTS}" ] && OBJECTS=$( get_objects "${IFILE}" ${DOMAIN} ${TYPE} )

ARR_OBJS=( $( separate_objects ${OBJECTS} ) )

NREQ="${#ARR_OBJS[@]}"
#echo Number of total requests: ${NREQ}
#echo

TMP_RETRIES=${N_RETRIES}

while [ ${#ARR_OBJS[@]} -gt 0 ]
do
  objs="${ARR_OBJS[0]}" # get first element

  BODY=$( mk_body "${TYPE}" "${objs}" "${DOMAIN}" )
  [ $(echo -ne "${BODY}" | wc -c ) -gt 131072 ] && { >&2 echo "ERROR: The body size is greater than 131072!!!" exit 1; }

  TIMESTAMP=$( date -u +'%Y%m%dT%H:%M:%S%z' )
  NONCE=$( mk_nonce ${TIMESTAMP} )
  SIGN_KEY=$( base64_hmac_sha256 $( aa_get client_secret "${AKA_PROPS[@]}" ) ${TIMESTAMP} )
  AUTH_HEADER=$( mk_auth_header ${TIMESTAMP} ${NONCE} "${AKA_PROPS[@]}" )

  declare -a DATA_TO_SIGN

  DATA_TO_SIGN=( $(aa_set method       POST "${DATA_TO_SIGN[@]}" ) )
  DATA_TO_SIGN=( $(aa_set scheme       https "${DATA_TO_SIGN[@]}" ) )
  DATA_TO_SIGN=( $(aa_set host         $( aa_get host "${AKA_PROPS[@]}" ) "${DATA_TO_SIGN[@]}" ) )
  DATA_TO_SIGN=( $(aa_set request_uri  "/ccu/v3/${ACTION}/${TYPE}/${NETWORK}" "${DATA_TO_SIGN[@]}" ) )
  DATA_TO_SIGN=( $(aa_set hash_content $( base64_sha256 "${BODY}" ) "${DATA_TO_SIGN[@]}" ) )
  DATA_TO_SIGN=( $(aa_set auth_header  ${AUTH_HEADER} "${DATA_TO_SIGN[@]}" ) )

  SIGNED_DATA=$( sign_data "${SIGN_KEY}" "${DATA_TO_SIGN[@]}" )
  SIGNED_AUTH_HEADER="Authorization: EG1-HMAC-SHA256 ${AUTH_HEADER}signature=${SIGNED_DATA}"

  H_JSON="Content-Type: application/json"

  if [ ${TMP_RETRIES} -le 1 ]; then
    ARR_OBJS=( "${ARR_OBJS[@]:1}" )
    TMP_RETRIES=${N_RETRIES};
    echo "ERROR: This request as aborted, max number of retries exceeded.";
    echo
    continue;
  fi

#  if [ ${TMP_RETRIES} -eq ${N_RETRIES} ]; then
#   echo "Number of remaining requests: ${#ARR_OBJS[@]}"
#   echo
#  fi

  REQ_CURL=$( echo curl -s -w \"\\n%{http_code}\\n\" \
                           -H \"Expect:\" \
                           -H \"User-Agent:${P_NAME} v${VERSION}\" \
                           -H \"Accept:${H_JSON}\" \
                           -H \"${H_JSON}\" \
                           -H \"${SIGNED_AUTH_HEADER}\" \
                           -X POST -d \'${BODY}\' \"$( aa_get scheme "${DATA_TO_SIGN[@]}" )://$( aa_get host "${DATA_TO_SIGN[@]}" )$( aa_get request_uri "${DATA_TO_SIGN[@]}" )\" )

  [ ${S_QUEST} -eq 1 ] && JSON_RES=$( echo "${REQ_CURL}" | bash -x ) || JSON_RES=$( echo "${REQ_CURL}" | bash -x 2> /dev/null )

  HTTP_CODE=$( echo "${JSON_RES}" | tail -1 )
  case "${HTTP_CODE}" in
    "201" )
      ARR_OBJS=( "${ARR_OBJS[@]:1}" ) # pop first element
    ;;
    "429" )
      TMP_RETRIES=$(( TMP_RETRIES - 1 ))
      echo
      countdown ${T_RETRY} "ERROR: Akamai purge rate limit (code ${HTTP_CODE}) -> (${TMP_RETRIES}) Retry request in"
      echo
      echo
      continue
    ;;
    "507" )
      TMP_RETRIES=$(( TMP_RETRIES - 1 ))
      echo
      countdown ${T_RETRY} "ERROR: Akamai purge queue limit (code ${HTTP_CODE}) -> (${TMP_RETRIES}) Retry request in"
      echo
      echo
      continue
    ;;
    "400" )
      echo
      echo "ABORT: Akamai bad request (code ${HTTP_CODE}) -> Check your parameters"
      echo
      echo
      exit 1
    ;;
    "401" )
      echo
      echo "ABORT: Akamai credential problem (code ${HTTP_CODE}) -> Check credentials used to connect to the edge server"
      echo
      echo
      exit 1
    ;;
    "403" )
      echo
      echo "ABORT: Akamai authorization token problem (code ${HTTP_CODE}) -> Check the authorizations for the client and the property configurations being purged"
      echo
      echo
      exit 1
    ;;
    "413" )
      echo
      echo "ABORT: Akamai request entity too large problem (code ${HTTP_CODE}) -> AHHHH Problem with script function separate_objects"
      echo
      echo
      exit 1
    ;;
    * )
      echo
      echo "ABORT: Akamai unknow problem (code ${HTTP_CODE})"
      echo
      echo
      exit 1
    ;;
  esac

  JSON_RESP=$( echo "${JSON_RES}" | sed '$ d' )

  which jq > /dev/null 2>&1
#  if [ ${?} -ne 0 ]; then
#    echo
#    echo "INFO: For better formatting to the output, install jq tool (https://stedolan.github.io/jq/)"
#    echo
#    echo "${JSON_RESP}"
#  else
#    echo
#    echo "${JSON_RESP}" | jq .
#  fi

done

exit 0
