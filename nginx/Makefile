CERT = nginx.pem
KEY = nginx.key
NGINX_CONF = nginx.conf
NGINX_TEMPLATE = nginx.conf.template
HTTP_TEST_URL = http://localhost:8080
HTTPS_TEST_URL = https://localhost:8443

.PHONY: all clean start test-http test-https

all: $(CERT) $(NGINX_CONF)

$(CERT): $(KEY)
	@echo "Certificate $(CERT) already exists."

$(KEY):
	openssl req -subj '/CN=localhost' -x509 -new -newkey rsa:4096 -keyout $(KEY) -out $(CERT) -sha256 -days 365 -nodes -addext "keyUsage = digitalSignature,keyAgreement" -addext "extendedKeyUsage = serverAuth, clientAuth" -addext "subjectAltName = DNS:ssl.test.local, DNS:localhost, IP:127.0.0.1"

$(NGINX_CONF): $(NGINX_TEMPLATE)
	rm -f $(NGINX_CONF)
	envsubst '$${PWD}' < $(NGINX_TEMPLATE) > $(NGINX_CONF)

start: all
	nginx -c $(PWD)/$(NGINX_CONF) -g 'daemon off;'

http:
	curl -v $(HTTP_TEST_URL)

https:
	curl -v --cacert $(CERT) $(HTTPS_TEST_URL)

clean:
	rm -f $(CERT) $(KEY) $(NGINX_CONF)

