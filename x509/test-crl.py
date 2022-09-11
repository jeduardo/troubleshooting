#!/usr/bin/env python

# Random script created to troubleshoot https://github.com/saltstack/salt/issues/47984

# Heavily inspired from https://github.com/pyca/pyopenssl/blob/master/tests/test_crypto.py

from OpenSSL.crypto import (
    X509Store,
    X509StoreFlags,
    X509StoreContext,
)
from OpenSSL.crypto import FILETYPE_PEM
from OpenSSL.crypto import CRL, Revoked, dump_crl
from OpenSSL.crypto import load_certificate, load_privatekey


def load_file(file):
    with open(file, 'r') as f:
        return f.read()


def save_crl(crl, path):
    file = '/tmp/%s' % path
    with open(file, 'w') as f:
        f.write(dump_crl(FILETYPE_PEM, crl))
    print('CRL saved as %s' % file)


def validate_cert(store, cert):
    store_ctx = X509StoreContext(store, cert)
    subject = cert.get_subject()
    try:
        store_ctx.verify_certificate()
        print('certificate %s is valid against CRL' % subject)
    except Exception as e:
        print('certificate %s is NOT valid against CRL' % subject)
        print(e)


def make_test_crl(issuer_cert, issuer_key, certs=()):
    """
    Create a CRL.
    :param list[X509] certs: A list of certificates to revoke.
    :rtype: CRL
    """
    crl = CRL()
    for cert in certs:
        revoked = Revoked()
        serial = hex(cert.get_serial_number())[2:].encode('utf-8')
        revoked.set_serial(serial)
        revoked.set_reason(b'cessationOfOperation')
        revoked.set_rev_date(b'20180601000000Z')
        crl.add_revoked(revoked)
    crl.set_version(1)
    crl.set_lastUpdate(b'20180601000000Z')
    crl.set_nextUpdate(b'20190601000000Z')
    crl.sign(issuer_cert, issuer_key, digest=b'sha512')
    return crl


# Constants
ROOT_CA_CERT = '/srv/pki/rootca.pem'
ROOT_CA_KEY = '/srv/pki/rootca.key'
INT_CA_CERT = '/srv/pki/intermediateca.pem'
INT_CA_KEY = '/srv/pki/intermediateca.key'
CERT1_CERT = '/srv/certs/salt1.dev.infra.network.pem'
CERT1_KEY = '/srv/certs/salt1.dev.infra.network.key'

# Load certificates from disk
root_ca_pem = load_file(ROOT_CA_CERT)
root_ca_key = load_file(ROOT_CA_KEY)
int_ca_cert_pem = load_file(INT_CA_CERT)
int_ca_cert_key = load_file(INT_CA_KEY)
cert1_pem = load_file(CERT1_CERT)
cert1_key = load_file(CERT1_KEY)

# Loading certificates from data
root_ca_cert = load_certificate(FILETYPE_PEM, root_ca_pem)
root_ca_key = load_privatekey(FILETYPE_PEM, root_ca_key)
int_ca_cert = load_certificate(FILETYPE_PEM, int_ca_cert_pem)
int_ca_key = load_privatekey(FILETYPE_PEM, int_ca_cert_key)
cert1 = load_certificate(FILETYPE_PEM, cert1_pem)
cert1_key = load_privatekey(FILETYPE_PEM, cert1_key)

# Creating certificate store in memory
store = X509Store()
store.add_cert(root_ca_cert)
store.add_cert(int_ca_cert)

# Creating CRL in memory revoking the first certificate, which was
# issued by the intermediate CA
crl = make_test_crl(int_ca_cert, int_ca_key, certs=[cert1])
store.add_crl(crl)
store.set_flags(X509StoreFlags.CRL_CHECK)

# Exporting CRL to disk for later analysis
save_crl(crl, 'intermediate.crl')

print('Testing revoked certificate for validation')
validate_cert(store, cert1)
