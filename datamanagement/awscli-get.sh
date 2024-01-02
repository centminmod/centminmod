#!/bin/bash
######################################################################
# install aws-cli & s5cmd
# written by George Liu (eva2000) centminmod.com
# https://github.com/peak/s5cmd
# https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-files.html#cli-configure-files-global
# https://docs.aws.amazon.com/cli/latest/userguide/install-cliv2.html
# https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-quickstart.html
# https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-profiles.html
# https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-services.html
# https://docs.aws.amazon.com/cli/latest/userguide/cli-usage-parameters-types.html
#
# expires
# --expires $(date -d "next hour" +%s)
#
# backblaze s3 api compatible
# aws configure --profile b2
# or
# aws configure set aws_access_key_id {keyID} --profile b2
# aws configure set aws_secret_access_key {applicationKey} --profile b2
# aws s3 ls --profile b2 --endpoint-url=https://s3.us-west-001.backblazeb2.com s3://your_b2_bucket
#
# s5cmd
# https://medium.com/@joshua_robinson/s5cmd-hits-v1-0-and-intro-to-advanced-usage-37ad02f7e895
# s5cmd --endpoint-url=https://s3.us-west-001.backblazeb2.com ls s3://your_b2_bucket
#
# https://help.backblaze.com/hc/en-us/articles/360047779633-Configuring-the-AWS-CLI-for-use-with-B2
# https://help.backblaze.com/hc/en-us/articles/360047425453
# https://www.backblaze.com/b2/docs/s3_compatible_api.html
#
# wasabi s3 api compatible
# https://wasabi-support.zendesk.com/hc/en-us/articles/115001910791-How-do-I-use-AWS-CLI-with-Wasabi-
# https://wasabi-support.zendesk.com/hc/en-us/articles/360044600552-How-do-I-use-s5cmd-with-Wasabi-
#
# s3 region list
# https://docs.aws.amazon.com/general/latest/gr/s3.html
# Africa (Cape Town): af-south-1
# Asia Pacific (Hong Kong): ap-east-1
# Asia Pacific (Mumbai): ap-south-1
# Asia Pacific (Osaka-Local): ap-northeast-3
# Asia Pacific (Seoul): ap-northeast-2
# Asia Pacific (Singapore): ap-southeast-1
# Asia Pacific (Sydney): ap-southeast-2
# Asia Pacific (Tokyo): ap-northeast-1
# AWS GovCloud (US-East): us-gov-east-1
# AWS GovCloud (US): us-gov-west-1
# Canada (Central): ca-central-1
# China (Beijing): cn-north-1
# China (Ningxia): cn-northwest-1
# Europe (Frankfurt): eu-central-1
# Europe (Ireland): eu-west-1
# Europe (London): eu-west-2
# Europe (Milan): eu-south-1
# Europe (Paris): eu-west-3
# Europe (Stockholm): eu-north-1
# Middle East (Bahrain): me-south-1
# South America (São Paulo): sa-east-1
# US East (N. Virginia): us-east-1
# US East (Ohio): us-east-2
# US West (N. California): us-west-1
# US West (Oregon): us-west-2
######################################################################
# variables
#############
DT=$(date +"%d%m%y-%H%M%S")
AWSCLI_DOWNLOAD='https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip'
AWSCLI_INSTALLDIR='/usr/local/aws-cli'
AWSCLI_BINPATH='/usr/local/bin'

SFIVECMD_VER='2.0.0'
######################################################################
# functions
#############

if [ ! -f /usr/bin/unzip ]; then
  yum -y -q install unzip
fi
if [ ! -f /usr/bin/jq ]; then
  yum -y -q install epel-release
  yum -y -q install jq
fi

help_text() {
  echo
  echo "Usage:"
  echo
  echo "$0 {install|update} {default|profilename|defaultreset} AWS_ACCESS_KEY_ID AWS_SECRET_ACCESS_KEY AWS_DEFAULT_REGION AWS_DEFAULT_OUTPUT"
  echo "$0 regions"
  echo "$0 regions-wasabi"
  exit
}

sfive_cmd() {
  echo
  echo "install s5cmd https://github.com/peak/s5cmd"
  mkdir -p /root/tools/awscli
  cd /root/tools/awscli
  wget -q -4 "https://github.com/peak/s5cmd/releases/download/v${SFIVECMD_VER}/s5cmd_${SFIVECMD_VER}_Linux-64bit.tar.gz"
  tar xzf "s5cmd_${SFIVECMD_VER}_Linux-64bit.tar.gz" -C /usr/local/bin/ s5cmd
  rm -f "s5cmd_${SFIVECMD_VER}_Linux-64bit.tar.gz"
  /usr/local/bin/s5cmd version
  echo
  echo "use AWS_PROFILE environment for multiple AWS profiles"
  echo "examples:"
  echo "export AWS_PROFILE=myprofile"
  echo "export AWS_PROFILE=b2"
  echo "export AWS_PROFILE=default"
}

list_regions() {
  echo "Africa (Cape Town): af-south-1"
  echo "Asia Pacific (Hong Kong): ap-east-1"
  echo "Asia Pacific (Mumbai): ap-south-1"
  echo "Asia Pacific (Osaka-Local): ap-northeast-3"
  echo "Asia Pacific (Seoul): ap-northeast-2"
  echo "Asia Pacific (Singapore): ap-southeast-1"
  echo "Asia Pacific (Sydney): ap-southeast-2"
  echo "Asia Pacific (Tokyo): ap-northeast-1"
  echo "AWS GovCloud (US-East): us-gov-east-1"
  echo "AWS GovCloud (US): us-gov-west-1"
  echo "Canada (Central): ca-central-1"
  echo "China (Beijing): cn-north-1"
  echo "China (Ningxia): cn-northwest-1"
  echo "Europe (Frankfurt): eu-central-1"
  echo "Europe (Ireland): eu-west-1"
  echo "Europe (London): eu-west-2"
  echo "Europe (Milan): eu-south-1"
  echo "Europe (Paris): eu-west-3"
  echo "Europe (Stockholm): eu-north-1"
  echo "Middle East (Bahrain): me-south-1"
  echo "South America (São Paulo): sa-east-1"
  echo "US East (N. Virginia): us-east-1"
  echo "US East (Ohio): us-east-2"
  echo "US West (N. California): us-west-1"
  exit
}

list_regions_wasabi() {
  echo "Wasabi US East 1 (N. Virginia): s3.wasabisys.com or s3.us-east-1.wasabisys.com"
  echo "Wasabi US East 2 (N. Virginia): s3.us-east-2.wasabisys.com "
  echo "Wasabi US West 1 (Oregon): s3.us-west-1.wasabisys.com"
  echo "Wasabi EU Central 1 (Amsterdam): s3.eu-central-1.wasabisys.com"
}

r2_config() {
  r2config_awsregion=$1
  r2config_profileid=$2
  if [[ "$r2config_awsregion" = 'r2' || "$r2config_awsregion" = 'auto' ]]; then
     if [ "$r2config_profileid" ]; then
       profileopt=" --profile $r2config_profileid"
     else
       profileopt=
     fi
     echo "configure aws cli for Cloudflare R2"
     # max_concurrent_requests = 2
     # aws configure get s3.max_concurrent_requests
     r2_max_concurrent_requests='2'
     echo "aws configure set s3.max_concurrent_requests ${r2_max_concurrent_requests}${profileopt}"
     aws configure set s3.max_concurrent_requests ${r2_max_concurrent_requests}${profileopt}
     # multipart_threshold = 50MB
     # aws configure get s3.multipart_threshold
     r2_multipart_threshold='50MB'
     echo "aws configure set s3.multipart_threshold ${r2_multipart_threshold}${profileopt}"
     aws configure set s3.multipart_threshold ${r2_multipart_threshold}${profileopt}
     # multipart_chunksize = 50MB
     # aws configure get s3.multipart_chunksize
     r2_multipart_chunksize='50MB'
     echo "aws configure set s3.multipart_chunksize ${r2_multipart_chunksize}${profileopt}"
     aws configure set s3.multipart_chunksize ${r2_multipart_chunksize}${profileopt}
     # addressing_style = path
     # aws configure get s3.addressing_style
     r2_addressing_style='path'
     echo "aws configure set s3.addressing_style ${r2_addressing_style}${profileopt}"
     aws configure set s3.addressing_style ${r2_addressing_style}${profileopt}
     # region = auto
     r2_region='auto'
     echo "aws configure set region ${r2_region}${profileopt}"
     aws configure set region ${r2_region}${profileopt}
  fi
}

getcli() {
  # accepted modes = install, update, help, configure
  mode=$1
  profileid=$2
  awskey=$3
  secretkey=$4
  awsregion=$5
  # https://docs.aws.amazon.com/cli/latest/userguide/cli-usage-output.html
  outputformat=$6
  awsconfigdir='/root/.aws'
  awsconfig="${awsconfigdir}/config"
  awscredentials="${awsconfigdir}/credentials"
  if [ ! -f /usr/bin/jq ]; then
    yum -q -y install jq
  fi
  if [ ! -f /usr/bin/sudo ]; then
    yum -q -y install sudo
  fi
  if [[ "$mode" = 'help' ]]; then
    help_text
  fi
  if [[ "$mode" = 'regions' ]]; then
    list_regions | column -s : -t
  fi
  if [[ "$mode" = 'regions-wasabi' ]]; then
    list_regions_wasabi | column -s : -t
  fi
  if [[ "$awsregion" = 'r2' || "$awsregion" = 'auto' ]]; then
    export AWS_DEFAULT_REGION='auto'
  fi
  if [[ "$mode" = 'install' || "$mode" = 'update' ]]; then
    if [ ! -f "${AWSCLI_BINPATH}/aws" ]; then
      echo
      echo "install aws-cli"
      mkdir -p /root/tools/awscli
      cd /root/tools/awscli
      rm -rf aws
      curl -4s "$AWSCLI_DOWNLOAD" -o "awscliv2.zip"
      unzip awscliv2.zip
      sudo ./aws/install -i "$AWSCLI_INSTALLDIR" -b "${AWSCLI_BINPATH}"
      chmod +x "${AWSCLI_BINPATH}/aws"
      rm -rf awscliv2.zip
      "${AWSCLI_BINPATH}/aws" --version
      sfive_cmd
    elif [[ "$mode" = 'update' && -f "${AWSCLI_BINPATH}/aws" ]]; then
      echo
      echo "update aws-cli"
      mkdir -p /root/tools/awscli
      cd /root/tools/awscli
      rm -rf aws
      curl -4s "$AWSCLI_DOWNLOAD" -o "awscliv2.zip"
      unzip awscliv2.zip
      # skip update if aws binary is same version of locally installed one
      localbinver=$("${AWSCLI_BINPATH}/aws" --version| awk '{print $1}')
      downloadbinver=$(aws/dist/aws --version| awk '{print $1}')
      if [[ "$downloadbinver" != "$localbinver" ]]; then
        sudo ./aws/install -i "$AWSCLI_INSTALLDIR" -b "${AWSCLI_BINPATH}" --update
        chmod +x "${AWSCLI_BINPATH}/aws"
        rm -rf awscliv2.zip
        "${AWSCLI_BINPATH}/aws" --version
      elif [[ "$downloadbinver" != "localbinver" ]]; then
        echo "skip update as downloaded version is the same"
      fi
      sfive_cmd
    fi
    if [[ ! "$awskey" || ! "$secretkey" ]]; then
      echo
      echo "Error. AWS Key & Secret not detected"
      echo "export relevant environment variables"
      echo "and re-run script"
      echo
      echo "example:"
      echo
      echo "export AWS_ACCESS_KEY_ID=AKIAIOSFODNN7EXAMPLE"
      echo "export AWS_SECRET_ACCESS_KEY=wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY"
      echo "export AWS_DEFAULT_REGION=us-west-2"
      echo "export AWS_DEFAULT_OUTPUT=text"
      exit 1
    fi
    if [[ -f "$awsconfig" && -f "$awscredentials" ]]; then
      echo
      echo "existing config file detected: $awsconfig"
      echo "existing credential file detected: $awscredentials"
    fi
    if [[ "$profileid" = 'default' || "$profileid" = 'defaultreset' ]]; then
      chkgetkey=$(${AWSCLI_BINPATH}/aws configure get aws_access_key_id >/dev/null 2>&1; echo $?)
      chkgetsecret=$(${AWSCLI_BINPATH}/aws configure get aws_secret_access_key >/dev/null 2>&1; echo $?)
      chkgetregion=$(${AWSCLI_BINPATH}/aws configure get default.region >/dev/null 2>&1; echo $?)
      chkgetoutput=$(${AWSCLI_BINPATH}/aws configure get output >/dev/null 2>&1; echo $?)
      if [[ "$profileid" = 'defaultreset' ]]; then
        echo
        echo "reset default aws-cli profile"
        ${AWSCLI_BINPATH}/aws configure set aws_access_key_id "$awskey"
        ${AWSCLI_BINPATH}/aws configure set aws_secret_access_key "$secretkey"
        ${AWSCLI_BINPATH}/aws configure set default.region "$awsregion"
        ${AWSCLI_BINPATH}/aws configure set output "$outputformat"
        r2_config "$awsregion"
      elif [[ "$profileid" = 'default' ]]; then
        if [[ "$chkgetkey" -ne '0' ]] || [[ ! -f "$awsconfig" || ! -f "$awscredentials" ]]; then
          # only configure if there is not existing values or config files do not exist
          echo
          echo "configure default aws-cli profile"
          ${AWSCLI_BINPATH}/aws configure set aws_access_key_id "$awskey"
          ${AWSCLI_BINPATH}/aws configure set aws_secret_access_key "$secretkey"
          ${AWSCLI_BINPATH}/aws configure set default.region "$awsregion"
          ${AWSCLI_BINPATH}/aws configure set output "$outputformat"
          r2_config "$awsregion"
        else
          skipconfig='y'
          echo
          echo -e "skipping configuration...\n"
          echo "detected existing default credentials"
          echo "in config file: $awsconfig"
          echo "in credential file: $awscredentials"
          echo
          echo -e "To override the default profile, use defaultreset mode:\n"
          echo "export AWS_ACCESS_KEY_ID=AKIAIOSFODNN7EXAMPLE"
          echo "export AWS_SECRET_ACCESS_KEY=wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY"
          echo "export AWS_DEFAULT_REGION=us-west-2"
          echo "export AWS_DEFAULT_OUTPUT=text"
          echo
          echo "$0 install defaultreset"
        fi
      fi
      if [[ "$profileid" = 'defaultreset' ]] || [[ "$profileid" = 'default' && "$skipconfig" != 'y' ]]; then
        getkey=$(${AWSCLI_BINPATH}/aws configure get aws_access_key_id)
        getsecret=$(${AWSCLI_BINPATH}/aws configure get aws_secret_access_key)
        getregion=$(${AWSCLI_BINPATH}/aws configure get default.region)
        getoutput=$(${AWSCLI_BINPATH}/aws configure get output)
        echo -e "\naws-cli profile: $profileid set:\n"
        echo "aws_access_key_id: $getkey"
        echo "aws_secret_access_key: $getsecret"
        echo "default.region: $getregion"
        echo "default output format: $getoutput"
      fi
    elif [[ "$profileid" != 'default' ]]; then
      echo
      echo "configure aws-cli profile: $profileid"
      ${AWSCLI_BINPATH}/aws configure set aws_access_key_id $awskey --profile "$profileid"
      ${AWSCLI_BINPATH}/aws configure set aws_secret_access_key $secretkey --profile "$profileid"
      ${AWSCLI_BINPATH}/aws configure set default.region $awsregion --profile "$profileid"
      ${AWSCLI_BINPATH}/aws configure set output $outputformat --profile "$profileid"
      r2_config "$awsregion" "$profileid"

      getkey=$(${AWSCLI_BINPATH}/aws configure get aws_access_key_id --profile "$profileid")
      getsecret=$(${AWSCLI_BINPATH}/aws configure get aws_secret_access_key --profile "$profileid")
      getregion=$(${AWSCLI_BINPATH}/aws configure get default.region --profile "$profileid")
      getoutput=$(${AWSCLI_BINPATH}/aws configure get output --profile "$profileid")
      echo -e "\naws-cli profile: $profileid set:\n"
      echo "aws_access_key_id: $getkey"
      echo "aws_secret_access_key: $getsecret"
      echo "default.region: $getregion"
      echo "default output format: $getoutput"
      echo
      echo -e "list aws-cli profiles:\n"
      ${AWSCLI_BINPATH}/aws configure list-profiles
    fi
  fi # install mode
}

######################################################################
# 1 - help or install
# 2 - default profile or specific profile
# 3 - AWS Access Key ID
# 4 - AWS Secret Access Key
# 5 - Default region name i.e. us-west-2, us-east-1
# 6 - Default output format i.e. json or text
#######################################################################
# supports detecting environment variables
# export AWS_ACCESS_KEY_ID=AKIAIOSFODNN7EXAMPLE
# export AWS_SECRET_ACCESS_KEY=wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY
# export AWS_DEFAULT_REGION=us-west-2
# export AWS_DEFAULT_OUTPUT=text
#######################################################################
getcli "${1:-install}" "${2:-default}" "${3:-$AWS_ACCESS_KEY_ID}" "${4:-$AWS_SECRET_ACCESS_KEY}" "${5:-$AWS_DEFAULT_REGION}" "${6:-$AWS_DEFAULT_OUTPUT}"