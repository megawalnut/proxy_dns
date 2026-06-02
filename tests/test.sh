domains=(
"cloudflare.com"
"dns.google"
"github.com"
"stackoverflow.com"
"stackexchange.com"
"superuser.com"
"serverfault.com"
"wikipedia.org"
"wikimedia.org"
"mozilla.org"
"ubuntu.com"
"debian.org"
"kernel.org"
"archlinux.org"
"qt.io"
"python.org"
"cplusplus.com"
"cppreference.com"
"openai.com"
"chatgpt.com"
"microsoft.com"
"azure.com"
"office.com"
"outlook.com"
"apple.com"
"icloud.com"
"amazon.com"
"aws.amazon.com"
"reddit.com"
"linkedin.com"
"netflix.com"
"spotify.com"
"discord.com"
"telegram.org"
"zoom.us"
"dropbox.com"
"adobe.com"
"oracle.com"
"intel.com"
"amd.com"
"nvidia.com"
"docker.com"
"kubernetes.io"
"gitlab.com"
"bitbucket.org"
"atlassian.com"
"digitalocean.com"
"linode.com"
"akamai.com"
"fastly.com"
"nginx.org"
"apache.org"
"mysql.com"
"postgresql.org"
"mongodb.com"
"elastic.co"
"grafana.com"
"prometheus.io"
"jetbrains.com"
"php.net"
"golang.org"
"rust-lang.org"
"nodejs.org"
"npmjs.com"
"pypi.org"
"tensorflow.org"
"pytorch.org"
"huggingface.co"
"cnn.com"
"bbc.com"
"reuters.com"
"nytimes.com"
"theguardian.com"
"forbes.com"
"techcrunch.com"
"arstechnica.com"
"ycombinator.com"
"yandex.ru"
"vk.com"
"habr.com"
"lenta.ru"
"tinkoff.ru"
"sberbank.ru"
"gosuslugi.ru"
"ozon.ru"
"wildberries.ru"
"avito.ru"
"2gis.ru"
"rutube.ru"
"kaspersky.com"
)

types=(
"A"
"AAAA"
"NS"
"MX"
"TXT"
"CNAME"
)

echo "waiting..."

for type in "${types[@]}"; do

  sum=0
  count=0
  
  for domain in "${domains[@]}"; do

    time=$(dig "$domain" "$type" +tries=1 +time=1 | grep "Query time:" | awk '{print $4}')
    
    sum=$((time + sum))
    ((count++))
    
  done

  avg=$((sum / count))
  printf "%-11s %-3s %-10s\n" "Type: $type" " | " "Average query time: $avg ms"

done

