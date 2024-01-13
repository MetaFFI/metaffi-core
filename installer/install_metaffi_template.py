import base64
import io
import platform
import re
import shlex
import shutil
import sys
import ctypes
import os
import tempfile
import traceback
import typing
import zipfile
import subprocess
import urllib.request

windows_x64_zip = 'windows_x64_zip_data'
ubuntu_x64_zip = 'ubuntu_x64_zip_data'
windows_patched_signal_file = 'Ly8gQ29weXJpZ2h0IDIwMTEgVGhlIEdvIEF1dGhvcnMuIEFsbCByaWdodHMgcmVzZXJ2ZWQuDQovLyBVc2Ugb2YgdGhpcyBzb3VyY2UgY29kZSBpcyBnb3Zlcm5lZCBieSBhIEJTRC1zdHlsZQ0KLy8gbGljZW5zZSB0aGF0IGNhbiBiZSBmb3VuZCBpbiB0aGUgTElDRU5TRSBmaWxlLg0KDQpwYWNrYWdlIHJ1bnRpbWUNCg0KaW1wb3J0ICgNCgkiaW50ZXJuYWwvYWJpIg0KCSJ1bnNhZmUiDQopDQoNCmNvbnN0ICgNCglfU0VNX0ZBSUxDUklUSUNBTEVSUk9SUyA9IDB4MDAwMQ0KCV9TRU1fTk9HUEZBVUxURVJST1JCT1ggID0gMHgwMDAyDQoJX1NFTV9OT09QRU5GSUxFRVJST1JCT1ggPSAweDgwMDANCg0KCV9XRVJfRkFVTFRfUkVQT1JUSU5HX05PX1VJID0gMHgwMDIwDQopDQoNCmZ1bmMgcHJldmVudEVycm9yRGlhbG9ncygpIHsNCgllcnJvcm1vZGUgOj0gc3RkY2FsbDAoX0dldEVycm9yTW9kZSkNCglzdGRjYWxsMShfU2V0RXJyb3JNb2RlLCBlcnJvcm1vZGV8X1NFTV9GQUlMQ1JJVElDQUxFUlJPUlN8X1NFTV9OT0dQRkFVTFRFUlJPUkJPWHxfU0VNX05PT1BFTkZJTEVFUlJPUkJPWCkNCg0KCS8vIERpc2FibGUgV0VSIGZhdWx0IHJlcG9ydGluZyBVSS4NCgkvLyBEbyB0aGlzIGV2ZW4gaWYgV0VSIGlzIGRpc2FibGVkIGFzIGEgd2hvbGUsDQoJLy8gYXMgV0VSIG1pZ2h0IGJlIGVuYWJsZWQgbGF0ZXIgd2l0aCBzZXRUcmFjZWJhY2soIndlciIpDQoJLy8gYW5kIHdlIHN0aWxsIHdhbnQgdGhlIGZhdWx0IHJlcG9ydGluZyBVSSB0byBiZSBkaXNhYmxlZCBpZiB0aGlzIGhhcHBlbnMuDQoJdmFyIHdlcmZsYWdzIHVpbnRwdHINCglzdGRjYWxsMihfV2VyR2V0RmxhZ3MsIGN1cnJlbnRQcm9jZXNzLCB1aW50cHRyKHVuc2FmZS5Qb2ludGVyKCZ3ZXJmbGFncykpKQ0KCXN0ZGNhbGwxKF9XZXJTZXRGbGFncywgd2VyZmxhZ3N8X1dFUl9GQVVMVF9SRVBPUlRJTkdfTk9fVUkpDQp9DQoNCi8vIGVuYWJsZVdFUiByZS1lbmFibGVzIFdpbmRvd3MgZXJyb3IgcmVwb3J0aW5nIHdpdGhvdXQgZmF1bHQgcmVwb3J0aW5nIFVJLg0KZnVuYyBlbmFibGVXRVIoKSB7DQoJLy8gcmUtZW5hYmxlIFdpbmRvd3MgRXJyb3IgUmVwb3J0aW5nDQoJZXJyb3Jtb2RlIDo9IHN0ZGNhbGwwKF9HZXRFcnJvck1vZGUpDQoJaWYgZXJyb3Jtb2RlJl9TRU1fTk9HUEZBVUxURVJST1JCT1ggIT0gMCB7DQoJCXN0ZGNhbGwxKF9TZXRFcnJvck1vZGUsIGVycm9ybW9kZV5fU0VNX05PR1BGQVVMVEVSUk9SQk9YKQ0KCX0NCn0NCg0KLy8gaW4gc3lzX3dpbmRvd3NfMzg2LnMsIHN5c193aW5kb3dzX2FtZDY0LnMsIHN5c193aW5kb3dzX2FybS5zLCBhbmQgc3lzX3dpbmRvd3NfYXJtNjQucw0KZnVuYyBleGNlcHRpb250cmFtcCgpDQpmdW5jIGZpcnN0Y29udGludWV0cmFtcCgpDQpmdW5jIGxhc3Rjb250aW51ZXRyYW1wKCkNCmZ1bmMgc2lncmVzdW1lKCkNCg0KZnVuYyBpbml0RXhjZXB0aW9uSGFuZGxlcigpIHsNCglzdGRjYWxsMihfQWRkVmVjdG9yZWRFeGNlcHRpb25IYW5kbGVyLCAxLCBhYmkuRnVuY1BDQUJJMChleGNlcHRpb250cmFtcCkpDQoJaWYgX0FkZFZlY3RvcmVkQ29udGludWVIYW5kbGVyID09IG5pbCB8fCBHT0FSQ0ggPT0gIjM4NiIgew0KCQkvLyB1c2UgU2V0VW5oYW5kbGVkRXhjZXB0aW9uRmlsdGVyIGZvciB3aW5kb3dzLTM4NiBvcg0KCQkvLyBpZiBWZWN0b3JlZENvbnRpbnVlSGFuZGxlciBpcyB1bmF2YWlsYWJsZS4NCgkJLy8gbm90ZTogU2V0VW5oYW5kbGVkRXhjZXB0aW9uRmlsdGVyIGhhbmRsZXIgd29uJ3QgYmUgY2FsbGVkLCBpZiBkZWJ1Z2dpbmcuDQoJCXN0ZGNhbGwxKF9TZXRVbmhhbmRsZWRFeGNlcHRpb25GaWx0ZXIsIGFiaS5GdW5jUENBQkkwKGxhc3Rjb250aW51ZXRyYW1wKSkNCgl9IGVsc2Ugew0KCQlzdGRjYWxsMihfQWRkVmVjdG9yZWRDb250aW51ZUhhbmRsZXIsIDEsIGFiaS5GdW5jUENBQkkwKGZpcnN0Y29udGludWV0cmFtcCkpDQoJCXN0ZGNhbGwyKF9BZGRWZWN0b3JlZENvbnRpbnVlSGFuZGxlciwgMCwgYWJpLkZ1bmNQQ0FCSTAobGFzdGNvbnRpbnVldHJhbXApKQ0KCX0NCn0NCg0KLy8gaXNBYm9ydCByZXR1cm5zIHRydWUsIGlmIGNvbnRleHQgciBkZXNjcmliZXMgZXhjZXB0aW9uIHJhaXNlZA0KLy8gYnkgY2FsbGluZyBydW50aW1lLmFib3J0IGZ1bmN0aW9uLg0KLy8NCi8vZ286bm9zcGxpdA0KZnVuYyBpc0Fib3J0KHIgKmNvbnRleHQpIGJvb2wgew0KCXBjIDo9IHIuaXAoKQ0KCWlmIEdPQVJDSCA9PSAiMzg2IiB8fCBHT0FSQ0ggPT0gImFtZDY0IiB8fCBHT0FSQ0ggPT0gImFybSIgew0KCQkvLyBJbiB0aGUgY2FzZSBvZiBhbiBhYm9ydCwgdGhlIGV4Y2VwdGlvbiBJUCBpcyBvbmUgYnl0ZSBhZnRlcg0KCQkvLyB0aGUgSU5UMyAodGhpcyBkaWZmZXJzIGZyb20gVU5JWCBPU2VzKS4gTm90ZSB0aGF0IG9uIEFSTSwNCgkJLy8gdGhpcyBtZWFucyB0aGF0IHRoZSBleGNlcHRpb24gSVAgaXMgbm8gbG9uZ2VyIGFsaWduZWQuDQoJCXBjLS0NCgl9DQoJcmV0dXJuIGlzQWJvcnRQQyhwYykNCn0NCg0KLy8gaXNnb2V4Y2VwdGlvbiByZXBvcnRzIHdoZXRoZXIgdGhpcyBleGNlcHRpb24gc2hvdWxkIGJlIHRyYW5zbGF0ZWQNCi8vIGludG8gYSBHbyBwYW5pYyBvciB0aHJvdy4NCi8vDQovLyBJdCBpcyBub3NwbGl0IHRvIGF2b2lkIGdyb3dpbmcgdGhlIHN0YWNrIGluIGNhc2Ugd2UncmUgYWJvcnRpbmcNCi8vIGJlY2F1c2Ugb2YgYSBzdGFjayBvdmVyZmxvdy4NCi8vDQovL2dvOm5vc3BsaXQNCmZ1bmMgaXNnb2V4Y2VwdGlvbihpbmZvICpleGNlcHRpb25yZWNvcmQsIHIgKmNvbnRleHQpIGJvb2wgew0KCS8vIE9ubHkgaGFuZGxlIGV4Y2VwdGlvbiBpZiBleGVjdXRpbmcgaW5zdHJ1Y3Rpb25zIGluIEdvIGJpbmFyeQ0KCS8vIChub3QgV2luZG93cyBsaWJyYXJ5IGNvZGUpLg0KCS8vIFRPRE8obXdodWRzb24pOiBuZWVkcyB0byBsb29wIHRvIHN1cHBvcnQgc2hhcmVkIGxpYnMNCglpZiByLmlwKCkgPCBmaXJzdG1vZHVsZWRhdGEudGV4dCB8fCBmaXJzdG1vZHVsZWRhdGEuZXRleHQgPCByLmlwKCkgew0KCQlyZXR1cm4gZmFsc2UNCgl9DQoNCgkvLyBHbyB3aWxsIG9ubHkgaGFuZGxlIHNvbWUgZXhjZXB0aW9ucy4NCglzd2l0Y2ggaW5mby5leGNlcHRpb25jb2RlIHsNCglkZWZhdWx0Og0KCQlyZXR1cm4gZmFsc2UNCgljYXNlIF9FWENFUFRJT05fQUNDRVNTX1ZJT0xBVElPTjoNCgljYXNlIF9FWENFUFRJT05fSU5fUEFHRV9FUlJPUjoNCgljYXNlIF9FWENFUFRJT05fSU5UX0RJVklERV9CWV9aRVJPOg0KCWNhc2UgX0VYQ0VQVElPTl9JTlRfT1ZFUkZMT1c6DQoJY2FzZSBfRVhDRVBUSU9OX0ZMVF9ERU5PUk1BTF9PUEVSQU5EOg0KCWNhc2UgX0VYQ0VQVElPTl9GTFRfRElWSURFX0JZX1pFUk86DQoJY2FzZSBfRVhDRVBUSU9OX0ZMVF9JTkVYQUNUX1JFU1VMVDoNCgljYXNlIF9FWENFUFRJT05fRkxUX09WRVJGTE9XOg0KCWNhc2UgX0VYQ0VQVElPTl9GTFRfVU5ERVJGTE9XOg0KCWNhc2UgX0VYQ0VQVElPTl9CUkVBS1BPSU5UOg0KCWNhc2UgX0VYQ0VQVElPTl9JTExFR0FMX0lOU1RSVUNUSU9OOiAvLyBicmVha3BvaW50IGFycml2ZXMgdGhpcyB3YXkgb24gYXJtNjQNCgl9DQoJcmV0dXJuIHRydWUNCn0NCg0KY29uc3QgKA0KCWNhbGxiYWNrVkVIID0gaW90YQ0KCWNhbGxiYWNrRmlyc3RWQ0gNCgljYWxsYmFja0xhc3RWQ0gNCikNCg0KLy8gc2lnRmV0Y2hHU2FmZSBpcyBsaWtlIGdldGcoKSBidXQgd2l0aG91dCBwYW5pY2tpbmcNCi8vIHdoZW4gVExTIGlzIG5vdCBzZXQuDQovLyBPbmx5IGltcGxlbWVudGVkIG9uIHdpbmRvd3MvMzg2LCB3aGljaCBpcyB0aGUgb25seQ0KLy8gYXJjaCB0aGF0IGxvYWRzIFRMUyB3aGVuIGNhbGxpbmcgZ2V0ZygpLiBPdGhlcnMNCi8vIHVzZSBhIGRlZGljYXRlZCByZWdpc3Rlci4NCmZ1bmMgc2lnRmV0Y2hHU2FmZSgpICpnDQoNCmZ1bmMgc2lnRmV0Y2hHKCkgKmcgew0KCWlmIEdPQVJDSCA9PSAiMzg2IiB7DQoJCXJldHVybiBzaWdGZXRjaEdTYWZlKCkNCgl9DQoJcmV0dXJuIGdldGcoKQ0KfQ0KDQovLyBzaWd0cmFtcGdvIGlzIGNhbGxlZCBmcm9tIHRoZSBleGNlcHRpb24gaGFuZGxlciBmdW5jdGlvbiwgc2lndHJhbXAsDQovLyB3cml0dGVuIGluIGFzc2VtYmx5IGNvZGUuDQovLyBSZXR1cm4gRVhDRVBUSU9OX0NPTlRJTlVFX0VYRUNVVElPTiBpZiB0aGUgZXhjZXB0aW9uIGlzIGhhbmRsZWQsDQovLyBlbHNlIHJldHVybiBFWENFUFRJT05fQ09OVElOVUVfU0VBUkNILg0KLy8NCi8vIEl0IGlzIG5vc3BsaXQgZm9yIHRoZSBzYW1lIHJlYXNvbiBhcyBleGNlcHRpb25oYW5kbGVyLg0KLy8NCi8vZ286bm9zcGxpdA0KZnVuYyBzaWd0cmFtcGdvKGVwICpleGNlcHRpb25wb2ludGVycywga2luZCBpbnQpIGludDMyIHsNCglncCA6PSBzaWdGZXRjaEcoKQ0KCWlmIGdwID09IG5pbCB7DQoJCXJldHVybiBfRVhDRVBUSU9OX0NPTlRJTlVFX1NFQVJDSA0KCX0NCg0KCXZhciBmbiBmdW5jKGluZm8gKmV4Y2VwdGlvbnJlY29yZCwgciAqY29udGV4dCwgZ3AgKmcpIGludDMyDQoJc3dpdGNoIGtpbmQgew0KCWNhc2UgY2FsbGJhY2tWRUg6DQoJCWZuID0gZXhjZXB0aW9uaGFuZGxlcg0KCWNhc2UgY2FsbGJhY2tGaXJzdFZDSDoNCgkJZm4gPSBmaXJzdGNvbnRpbnVlaGFuZGxlcg0KCWNhc2UgY2FsbGJhY2tMYXN0VkNIOg0KCQlmbiA9IGxhc3Rjb250aW51ZWhhbmRsZXINCglkZWZhdWx0Og0KCQl0aHJvdygidW5rbm93biBzaWd0cmFtcCBjYWxsYmFjayIpDQoJfQ0KDQoJLy8gQ2hlY2sgaWYgd2UgYXJlIHJ1bm5pbmcgb24gZzAgc3RhY2ssIGFuZCBpZiB3ZSBhcmUsDQoJLy8gY2FsbCBmbiBkaXJlY3RseSBpbnN0ZWFkIG9mIGNyZWF0aW5nIHRoZSBjbG9zdXJlLg0KCS8vIGZvciB0aGUgc3lzdGVtc3RhY2sgYXJndW1lbnQuDQoJLy8NCgkvLyBBIGNsb3N1cmUgY2FuJ3QgYmUgbWFya2VkIGFzIG5vc3BsaXQsIHNvIGl0IG1pZ2h0DQoJLy8gY2FsbCBtb3Jlc3RhY2sgaWYgd2UgYXJlIGF0IHRoZSBnMCBzdGFjayBsaW1pdC4NCgkvLyBJZiB0aGF0IGhhcHBlbnMsIHRoZSBydW50aW1lIHdpbGwgY2FsbCBhYm9ydA0KCS8vIGFuZCBlbmQgdXAgaW4gc2lndHJhbXBnbyBhZ2Fpbi4NCgkvLyBUT0RPOiByZXZpc2l0IHRoaXMgd29ya2Fyb3VuZCBpZi93aGVuIGNsb3N1cmVzDQoJLy8gY2FuIGJlIGNvbXBpbGVkIGFzIG5vc3BsaXQuDQoJLy8NCgkvLyBOb3RlIHRoYXQgdGhpcyBzY2VuYXJpbyBzaG91bGQgb25seSBvY2N1ciBvbg0KCS8vIFRlc3RHMFN0YWNrT3ZlcmZsb3cuIEFueSBvdGhlciBvY2N1cnJlbmNlIHNob3VsZA0KCS8vIGJlIHRyZWF0ZWQgYXMgYSBidWcuDQoJdmFyIHJldCBpbnQzMg0KCWlmIGdwICE9IGdwLm0uZzAgew0KCQlzeXN0ZW1zdGFjayhmdW5jKCkgew0KCQkJcmV0ID0gZm4oZXAucmVjb3JkLCBlcC5jb250ZXh0LCBncCkNCgkJfSkNCgl9IGVsc2Ugew0KCQlyZXQgPSBmbihlcC5yZWNvcmQsIGVwLmNvbnRleHQsIGdwKQ0KCX0NCglpZiByZXQgPT0gX0VYQ0VQVElPTl9DT05USU5VRV9TRUFSQ0ggew0KCQlyZXR1cm4gcmV0DQoJfQ0KDQoJLy8gQ2hlY2sgaWYgd2UgbmVlZCB0byBzZXQgdXAgdGhlIGNvbnRyb2wgZmxvdyBndWFyZCB3b3JrYXJvdW5kLg0KCS8vIE9uIFdpbmRvd3MsIHRoZSBzdGFjayBwb2ludGVyIGluIHRoZSBjb250ZXh0IG11c3QgbGllIHdpdGhpbg0KCS8vIHN5c3RlbSBzdGFjayBsaW1pdHMgd2hlbiB3ZSByZXN1bWUgZnJvbSBleGNlcHRpb24uDQoJLy8gU3RvcmUgdGhlIHJlc3VtZSBTUCBhbmQgUEMgaW4gYWx0ZXJuYXRlIHJlZ2lzdGVycw0KCS8vIGFuZCByZXR1cm4gdG8gc2lncmVzdW1lIG9uIHRoZSBnMCBzdGFjay4NCgkvLyBzaWdyZXN1bWUgbWFrZXMgbm8gdXNlIG9mIHRoZSBzdGFjayBhdCBhbGwsDQoJLy8gbG9hZGluZyBTUCBmcm9tIFJYIGFuZCBqdW1waW5nIHRvIFJZLCBiZWluZyBSWCBhbmQgUlkgdHdvIHNjcmF0Y2ggcmVnaXN0ZXJzLg0KCS8vIE5vdGUgdGhhdCBibGluZGx5IHNtYXNoaW5nIFJYIGFuZCBSWSBpcyBvbmx5IHNhZmUgYmVjYXVzZSB3ZSBrbm93IHNpZ3BhbmljDQoJLy8gd2lsbCBub3QgYWN0dWFsbHkgcmV0dXJuIHRvIHRoZSBvcmlnaW5hbCBmcmFtZSwgc28gdGhlIHJlZ2lzdGVycw0KCS8vIGFyZSBlZmZlY3RpdmVseSBkZWFkLiBCdXQgdGhpcyBkb2VzIG1lYW4gd2UgY2FuJ3QgdXNlIHRoZQ0KCS8vIHNhbWUgbWVjaGFuaXNtIGZvciBhc3luYyBwcmVlbXB0aW9uLg0KCWlmIGVwLmNvbnRleHQuaXAoKSA9PSBhYmkuRnVuY1BDQUJJMChzaWdyZXN1bWUpIHsNCgkJLy8gc2lncmVzdW1lIGhhcyBhbHJlYWR5IGJlZW4gc2V0IHVwIGJ5IGEgcHJldmlvdXMgZXhjZXB0aW9uLg0KCQlyZXR1cm4gcmV0DQoJfQ0KCXByZXBhcmVDb250ZXh0Rm9yU2lnUmVzdW1lKGVwLmNvbnRleHQpDQoJZXAuY29udGV4dC5zZXRfc3AoZ3AubS5nMC5zY2hlZC5zcCkNCgllcC5jb250ZXh0LnNldF9pcChhYmkuRnVuY1BDQUJJMChzaWdyZXN1bWUpKQ0KCXJldHVybiByZXQNCn0NCg0KLy8gQ2FsbGVkIGJ5IHNpZ3RyYW1wIGZyb20gV2luZG93cyBWRUggaGFuZGxlci4NCi8vIFJldHVybiB2YWx1ZSBzaWduYWxzIHdoZXRoZXIgdGhlIGV4Y2VwdGlvbiBoYXMgYmVlbiBoYW5kbGVkIChFWENFUFRJT05fQ09OVElOVUVfRVhFQ1VUSU9OKQ0KLy8gb3Igc2hvdWxkIGJlIG1hZGUgYXZhaWxhYmxlIHRvIG90aGVyIGhhbmRsZXJzIGluIHRoZSBjaGFpbiAoRVhDRVBUSU9OX0NPTlRJTlVFX1NFQVJDSCkuDQovLw0KLy8gVGhpcyBpcyBub3NwbGl0IHRvIGF2b2lkIGdyb3dpbmcgdGhlIHN0YWNrIHVudGlsIHdlJ3ZlIGNoZWNrZWQgZm9yDQovLyBfRVhDRVBUSU9OX0JSRUFLUE9JTlQsIHdoaWNoIGlzIHJhaXNlZCBieSBhYm9ydCgpIGlmIHdlIG92ZXJmbG93IHRoZSBnMCBzdGFjay4NCi8vDQovL2dvOm5vc3BsaXQNCmZ1bmMgZXhjZXB0aW9uaGFuZGxlcihpbmZvICpleGNlcHRpb25yZWNvcmQsIHIgKmNvbnRleHQsIGdwICpnKSBpbnQzMiB7DQoNCglyZXR1cm4gX0VYQ0VQVElPTl9DT05USU5VRV9TRUFSQ0gNCg0KCS8vIGlmICFpc2dvZXhjZXB0aW9uKGluZm8sIHIpIHsNCgkvLyAJcmV0dXJuIF9FWENFUFRJT05fQ09OVElOVUVfU0VBUkNIDQoJLy8gfQ0KDQoJLy8gaWYgZ3AudGhyb3dzcGxpdCB8fCBpc0Fib3J0KHIpIHsNCgkvLyAJLy8gV2UgY2FuJ3Qgc2FmZWx5IHNpZ3BhbmljIGJlY2F1c2UgaXQgbWF5IGdyb3cgdGhlIHN0YWNrLg0KCS8vIAkvLyBPciB0aGlzIGlzIGEgY2FsbCB0byBhYm9ydC4NCgkvLyAJLy8gRG9uJ3QgZ28gdGhyb3VnaCBhbnkgbW9yZSBvZiB0aGUgV2luZG93cyBoYW5kbGVyIGNoYWluLg0KCS8vIAkvLyBDcmFzaCBub3cuDQoJLy8gCXdpbnRocm93KGluZm8sIHIsIGdwKQ0KCS8vIH0NCg0KCS8vIC8vIEFmdGVyIHRoaXMgcG9pbnQsIGl0IGlzIHNhZmUgdG8gZ3JvdyB0aGUgc3RhY2suDQoNCgkvLyAvLyBNYWtlIGl0IGxvb2sgbGlrZSBhIGNhbGwgdG8gdGhlIHNpZ25hbCBmdW5jLg0KCS8vIC8vIEhhdmUgdG8gcGFzcyBhcmd1bWVudHMgb3V0IG9mIGJhbmQgc2luY2UNCgkvLyAvLyBhdWdtZW50aW5nIHRoZSBzdGFjayBmcmFtZSB3b3VsZCBicmVhaw0KCS8vIC8vIHRoZSB1bndpbmRpbmcgY29kZS4NCgkvLyBncC5zaWcgPSBpbmZvLmV4Y2VwdGlvbmNvZGUNCgkvLyBncC5zaWdjb2RlMCA9IGluZm8uZXhjZXB0aW9uaW5mb3JtYXRpb25bMF0NCgkvLyBncC5zaWdjb2RlMSA9IGluZm8uZXhjZXB0aW9uaW5mb3JtYXRpb25bMV0NCgkvLyBncC5zaWdwYyA9IHIuaXAoKQ0KDQoJLy8gLy8gT25seSBwdXNoIHJ1bnRpbWXCt3NpZ3BhbmljIGlmIHIuaXAoKSAhPSAwLg0KCS8vIC8vIElmIHIuaXAoKSA9PSAwLCBwcm9iYWJseSBwYW5pY2tlZCBiZWNhdXNlIG9mIGENCgkvLyAvLyBjYWxsIHRvIGEgbmlsIGZ1bmMuIE5vdCBwdXNoaW5nIHRoYXQgb250byBzcCB3aWxsDQoJLy8gLy8gbWFrZSB0aGUgdHJhY2UgbG9vayBsaWtlIGEgY2FsbCB0byBydW50aW1lwrdzaWdwYW5pYyBpbnN0ZWFkLg0KCS8vIC8vIChPdGhlcndpc2UgdGhlIHRyYWNlIHdpbGwgZW5kIGF0IHJ1bnRpbWXCt3NpZ3BhbmljIGFuZCB3ZQ0KCS8vIC8vIHdvbid0IGdldCB0byBzZWUgd2hvIGZhdWx0ZWQuKQ0KCS8vIC8vIEFsc28gZG9uJ3QgcHVzaCBhIHNpZ3BhbmljIGZyYW1lIGlmIHRoZSBmYXVsdGluZyBQQw0KCS8vIC8vIGlzIHRoZSBlbnRyeSBvZiBhc3luY1ByZWVtcHQuIEluIHRoaXMgY2FzZSwgd2Ugc3VzcGVuZGVkDQoJLy8gLy8gdGhlIHRocmVhZCByaWdodCBiZXR3ZWVuIHRoZSBmYXVsdCBhbmQgdGhlIGV4Y2VwdGlvbiBoYW5kbGVyDQoJLy8gLy8gc3RhcnRpbmcgdG8gcnVuLCBhbmQgd2UgaGF2ZSBwdXNoZWQgYW4gYXN5bmNQcmVlbXB0IGNhbGwuDQoJLy8gLy8gVGhlIGV4Y2VwdGlvbiBpcyBub3QgZnJvbSBhc3luY1ByZWVtcHQsIHNvIG5vdCB0byBwdXNoIGENCgkvLyAvLyBzaWdwYW5pYyBjYWxsIHRvIG1ha2UgaXQgbG9vayBsaWtlIHRoYXQuIEluc3RlYWQsIGp1c3QNCgkvLyAvLyBvdmVyd3JpdGUgdGhlIFBDLiAoU2VlIGlzc3VlICMzNTc3MykNCgkvLyBpZiByLmlwKCkgIT0gMCAmJiByLmlwKCkgIT0gYWJpLkZ1bmNQQ0FCSTAoYXN5bmNQcmVlbXB0KSB7DQoJLy8gCXNwIDo9IHVuc2FmZS5Qb2ludGVyKHIuc3AoKSkNCgkvLyAJZGVsdGEgOj0gdWludHB0cihzeXMuU3RhY2tBbGlnbikNCgkvLyAJc3AgPSBhZGQoc3AsIC1kZWx0YSkNCgkvLyAJci5zZXRfc3AodWludHB0cihzcCkpDQoJLy8gCWlmIHVzZXNMUiB7DQoJLy8gCQkqKCgqdWludHB0cikoc3ApKSA9IHIubHIoKQ0KCS8vIAkJci5zZXRfbHIoci5pcCgpKQ0KCS8vIAl9IGVsc2Ugew0KCS8vIAkJKigoKnVpbnRwdHIpKHNwKSkgPSByLmlwKCkNCgkvLyAJfQ0KCS8vIH0NCgkvLyByLnNldF9pcChhYmkuRnVuY1BDQUJJMChzaWdwYW5pYzApKQ0KCS8vIHJldHVybiBfRVhDRVBUSU9OX0NPTlRJTlVFX0VYRUNVVElPTg0KfQ0KDQovLyBJdCBzZWVtcyBXaW5kb3dzIHNlYXJjaGVzIENvbnRpbnVlSGFuZGxlcidzIGxpc3QgZXZlbg0KLy8gaWYgRXhjZXB0aW9uSGFuZGxlciByZXR1cm5zIEVYQ0VQVElPTl9DT05USU5VRV9FWEVDVVRJT04uDQovLyBmaXJzdGNvbnRpbnVlaGFuZGxlciB3aWxsIHN0b3AgdGhhdCBzZWFyY2gsDQovLyBpZiBleGNlcHRpb25oYW5kbGVyIGRpZCB0aGUgc2FtZSBlYXJsaWVyLg0KLy8NCi8vIEl0IGlzIG5vc3BsaXQgZm9yIHRoZSBzYW1lIHJlYXNvbiBhcyBleGNlcHRpb25oYW5kbGVyLg0KLy8NCi8vZ286bm9zcGxpdA0KZnVuYyBmaXJzdGNvbnRpbnVlaGFuZGxlcihpbmZvICpleGNlcHRpb25yZWNvcmQsIHIgKmNvbnRleHQsIGdwICpnKSBpbnQzMiB7DQoNCglyZXR1cm4gX0VYQ0VQVElPTl9DT05USU5VRV9TRUFSQ0gNCg0KCS8vIGlmICFpc2dvZXhjZXB0aW9uKGluZm8sIHIpIHsNCgkvLyAJcmV0dXJuIF9FWENFUFRJT05fQ09OVElOVUVfU0VBUkNIDQoJLy8gfQ0KCS8vIHJldHVybiBfRVhDRVBUSU9OX0NPTlRJTlVFX0VYRUNVVElPTg0KfQ0KDQovLyBsYXN0Y29udGludWVoYW5kbGVyIGlzIHJlYWNoZWQsIGJlY2F1c2UgcnVudGltZSBjYW5ub3QgaGFuZGxlDQovLyBjdXJyZW50IGV4Y2VwdGlvbi4gbGFzdGNvbnRpbnVlaGFuZGxlciB3aWxsIHByaW50IGNyYXNoIGluZm8gYW5kIGV4aXQuDQovLw0KLy8gSXQgaXMgbm9zcGxpdCBmb3IgdGhlIHNhbWUgcmVhc29uIGFzIGV4Y2VwdGlvbmhhbmRsZXIuDQovLw0KLy9nbzpub3NwbGl0DQpmdW5jIGxhc3Rjb250aW51ZWhhbmRsZXIoaW5mbyAqZXhjZXB0aW9ucmVjb3JkLCByICpjb250ZXh0LCBncCAqZykgaW50MzIgew0KDQoJcmV0dXJuIF9FWENFUFRJT05fQ09OVElOVUVfU0VBUkNIDQoNCgkvLyBpZiBpc2xpYnJhcnkgfHwgaXNhcmNoaXZlIHsNCgkvLyAJLy8gR28gRExML2FyY2hpdmUgaGFzIGJlZW4gbG9hZGVkIGluIGEgbm9uLWdvIHByb2dyYW0uDQoJLy8gCS8vIElmIHRoZSBleGNlcHRpb24gZG9lcyBub3Qgb3JpZ2luYXRlIGZyb20gZ28sIHRoZSBnbyBydW50aW1lDQoJLy8gCS8vIHNob3VsZCBub3QgdGFrZSByZXNwb25zaWJpbGl0eSBvZiBjcmFzaGluZyB0aGUgcHJvY2Vzcy4NCgkvLyAJcmV0dXJuIF9FWENFUFRJT05fQ09OVElOVUVfU0VBUkNIDQoJLy8gfQ0KDQoJLy8gLy8gVkVIIGlzIGNhbGxlZCBiZWZvcmUgU0VILCBidXQgYXJtNjQgTVNWQyBETExzIHVzZSBTRUggdG8gdHJhcA0KCS8vIC8vIGlsbGVnYWwgaW5zdHJ1Y3Rpb25zIGR1cmluZyBydW50aW1lIGluaXRpYWxpemF0aW9uIHRvIGRldGVybWluZQ0KCS8vIC8vIENQVSBmZWF0dXJlcywgc28gaWYgd2UgbWFrZSBpdCB0byB0aGUgbGFzdCBoYW5kbGVyIGFuZCB3ZSdyZQ0KCS8vIC8vIGFybTY0IGFuZCBpdCdzIGFuIGlsbGVnYWwgaW5zdHJ1Y3Rpb24gYW5kIHRoaXMgaXMgY29taW5nIGZyb20NCgkvLyAvLyBub24tR28gY29kZSwgdGhlbiBhc3N1bWUgaXQncyB0aGlzIHJ1bnRpbWUgcHJvYmluZyBoYXBwZW4sIGFuZA0KCS8vIC8vIHBhc3MgdGhhdCBvbndhcmQgdG8gU0VILg0KCS8vIGlmIEdPQVJDSCA9PSAiYXJtNjQiICYmIGluZm8uZXhjZXB0aW9uY29kZSA9PSBfRVhDRVBUSU9OX0lMTEVHQUxfSU5TVFJVQ1RJT04gJiYNCgkvLyAJKHIuaXAoKSA8IGZpcnN0bW9kdWxlZGF0YS50ZXh0IHx8IGZpcnN0bW9kdWxlZGF0YS5ldGV4dCA8IHIuaXAoKSkgew0KCS8vIAlyZXR1cm4gX0VYQ0VQVElPTl9DT05USU5VRV9TRUFSQ0gNCgkvLyB9DQoNCgkvLyB3aW50aHJvdyhpbmZvLCByLCBncCkNCgkvLyByZXR1cm4gMCAvLyBub3QgcmVhY2hlZA0KfQ0KDQovLyBBbHdheXMgY2FsbGVkIG9uIGcwLiBncCBpcyB0aGUgRyB3aGVyZSB0aGUgZXhjZXB0aW9uIG9jY3VycmVkLg0KLy8NCi8vZ286bm9zcGxpdA0KZnVuYyB3aW50aHJvdyhpbmZvICpleGNlcHRpb25yZWNvcmQsIHIgKmNvbnRleHQsIGdwICpnKSB7DQoJZzAgOj0gZ2V0ZygpDQoNCglpZiBwYW5pY2tpbmcuTG9hZCgpICE9IDAgeyAvLyB0cmFjZWJhY2sgYWxyZWFkeSBwcmludGVkDQoJCWV4aXQoMikNCgl9DQoJcGFuaWNraW5nLlN0b3JlKDEpDQoNCgkvLyBJbiBjYXNlIHdlJ3JlIGhhbmRsaW5nIGEgZzAgc3RhY2sgb3ZlcmZsb3csIGJsb3cgYXdheSB0aGUNCgkvLyBnMCBzdGFjayBib3VuZHMgc28gd2UgaGF2ZSByb29tIHRvIHByaW50IHRoZSB0cmFjZWJhY2suIElmDQoJLy8gdGhpcyBzb21laG93IG92ZXJmbG93cyB0aGUgc3RhY2ssIHRoZSBPUyB3aWxsIHRyYXAgaXQuDQoJZzAuc3RhY2subG8gPSAwDQoJZzAuc3RhY2tndWFyZDAgPSBnMC5zdGFjay5sbyArIHN0YWNrR3VhcmQNCglnMC5zdGFja2d1YXJkMSA9IGcwLnN0YWNrZ3VhcmQwDQoNCglwcmludCgiRXhjZXB0aW9uICIsIGhleChpbmZvLmV4Y2VwdGlvbmNvZGUpLCAiICIsIGhleChpbmZvLmV4Y2VwdGlvbmluZm9ybWF0aW9uWzBdKSwgIiAiLCBoZXgoaW5mby5leGNlcHRpb25pbmZvcm1hdGlvblsxXSksICIgIiwgaGV4KHIuaXAoKSksICJcbiIpDQoNCglwcmludCgiUEM9IiwgaGV4KHIuaXAoKSksICJcbiIpDQoJaWYgZzAubS5pbmNnbyAmJiBncCA9PSBnMC5tLmcwICYmIGcwLm0uY3VyZyAhPSBuaWwgew0KCQlpZiBpc2NnbyB7DQoJCQlwcmludCgic2lnbmFsIGFycml2ZWQgZHVyaW5nIGV4dGVybmFsIGNvZGUgZXhlY3V0aW9uXG4iKQ0KCQl9DQoJCWdwID0gZzAubS5jdXJnDQoJfQ0KCXByaW50KCJcbiIpDQoNCglnMC5tLnRocm93aW5nID0gdGhyb3dUeXBlUnVudGltZQ0KCWcwLm0uY2F1Z2h0c2lnLnNldChncCkNCg0KCWxldmVsLCBfLCBkb2NyYXNoIDo9IGdvdHJhY2ViYWNrKCkNCglpZiBsZXZlbCA+IDAgew0KCQl0cmFjZWJhY2t0cmFwKHIuaXAoKSwgci5zcCgpLCByLmxyKCksIGdwKQ0KCQl0cmFjZWJhY2tvdGhlcnMoZ3ApDQoJCWR1bXByZWdzKHIpDQoJfQ0KDQoJaWYgZG9jcmFzaCB7DQoJCWRpZUZyb21FeGNlcHRpb24oaW5mbywgcikNCgl9DQoNCglleGl0KDIpDQp9DQoNCmZ1bmMgc2lncGFuaWMoKSB7DQoJZ3AgOj0gZ2V0ZygpDQoJaWYgIWNhbnBhbmljKCkgew0KCQl0aHJvdygidW5leHBlY3RlZCBzaWduYWwgZHVyaW5nIHJ1bnRpbWUgZXhlY3V0aW9uIikNCgl9DQoNCglzd2l0Y2ggZ3Auc2lnIHsNCgljYXNlIF9FWENFUFRJT05fQUNDRVNTX1ZJT0xBVElPTiwgX0VYQ0VQVElPTl9JTl9QQUdFX0VSUk9SOg0KCQlpZiBncC5zaWdjb2RlMSA8IDB4MTAwMCB7DQoJCQlwYW5pY21lbSgpDQoJCX0NCgkJaWYgZ3AucGFuaWNvbmZhdWx0IHsNCgkJCXBhbmljbWVtQWRkcihncC5zaWdjb2RlMSkNCgkJfQ0KCQlpZiBpblVzZXJBcmVuYUNodW5rKGdwLnNpZ2NvZGUxKSB7DQoJCQkvLyBXZSBjb3VsZCBjaGVjayB0aGF0IHRoZSBhcmVuYSBjaHVuayBpcyBleHBsaWNpdGx5IHNldCB0byBmYXVsdCwNCgkJCS8vIGJ1dCB0aGUgZmFjdCB0aGF0IHdlIGZhdWx0ZWQgb24gYWNjZXNzaW5nIGl0IGlzIGVub3VnaCB0byBwcm92ZQ0KCQkJLy8gdGhhdCBpdCBpcy4NCgkJCXByaW50KCJhY2Nlc3NlZCBkYXRhIGZyb20gZnJlZWQgdXNlciBhcmVuYSAiLCBoZXgoZ3Auc2lnY29kZTEpLCAiXG4iKQ0KCQl9IGVsc2Ugew0KCQkJcHJpbnQoInVuZXhwZWN0ZWQgZmF1bHQgYWRkcmVzcyAiLCBoZXgoZ3Auc2lnY29kZTEpLCAiXG4iKQ0KCQl9DQoJCXRocm93KCJmYXVsdCIpDQoJY2FzZSBfRVhDRVBUSU9OX0lOVF9ESVZJREVfQllfWkVSTzoNCgkJcGFuaWNkaXZpZGUoKQ0KCWNhc2UgX0VYQ0VQVElPTl9JTlRfT1ZFUkZMT1c6DQoJCXBhbmljb3ZlcmZsb3coKQ0KCWNhc2UgX0VYQ0VQVElPTl9GTFRfREVOT1JNQUxfT1BFUkFORCwNCgkJX0VYQ0VQVElPTl9GTFRfRElWSURFX0JZX1pFUk8sDQoJCV9FWENFUFRJT05fRkxUX0lORVhBQ1RfUkVTVUxULA0KCQlfRVhDRVBUSU9OX0ZMVF9PVkVSRkxPVywNCgkJX0VYQ0VQVElPTl9GTFRfVU5ERVJGTE9XOg0KCQlwYW5pY2Zsb2F0KCkNCgl9DQoJdGhyb3coImZhdWx0IikNCn0NCg0KLy8gRm9sbG93aW5nIGFyZSBub3QgaW1wbGVtZW50ZWQuDQoNCmZ1bmMgaW5pdHNpZyhwcmVpbml0IGJvb2wpIHsNCn0NCg0KZnVuYyBzaWdlbmFibGUoc2lnIHVpbnQzMikgew0KfQ0KDQpmdW5jIHNpZ2Rpc2FibGUoc2lnIHVpbnQzMikgew0KfQ0KDQpmdW5jIHNpZ2lnbm9yZShzaWcgdWludDMyKSB7DQp9DQoNCmZ1bmMgc2lnbmFtZShzaWcgdWludDMyKSBzdHJpbmcgew0KCXJldHVybiAiIg0KfQ0KDQovL2dvOm5vc3BsaXQNCmZ1bmMgY3Jhc2goKSB7DQoJZGllRnJvbUV4Y2VwdGlvbihuaWwsIG5pbCkNCn0NCg0KLy8gZGllRnJvbUV4Y2VwdGlvbiByYWlzZXMgYW4gZXhjZXB0aW9uIHRoYXQgYnlwYXNzZXMgYWxsIGV4Y2VwdGlvbiBoYW5kbGVycy4NCi8vIFRoaXMgcHJvdmlkZXMgdGhlIGV4cGVjdGVkIGV4aXQgc3RhdHVzIGZvciB0aGUgc2hlbGwuDQovLw0KLy9nbzpub3NwbGl0DQpmdW5jIGRpZUZyb21FeGNlcHRpb24oaW5mbyAqZXhjZXB0aW9ucmVjb3JkLCByICpjb250ZXh0KSB7DQoJaWYgaW5mbyA9PSBuaWwgew0KCQlncCA6PSBnZXRnKCkNCgkJaWYgZ3Auc2lnICE9IDAgew0KCQkJLy8gVHJ5IHRvIHJlY29uc3RydWN0IGFuIGV4Y2VwdGlvbiByZWNvcmQgZnJvbQ0KCQkJLy8gdGhlIGV4Y2VwdGlvbiBpbmZvcm1hdGlvbiBzdG9yZWQgaW4gZ3AuDQoJCQlpbmZvID0gJmV4Y2VwdGlvbnJlY29yZHsNCgkJCQlleGNlcHRpb25hZGRyZXNzOiBncC5zaWdwYywNCgkJCQlleGNlcHRpb25jb2RlOiAgICBncC5zaWcsDQoJCQkJbnVtYmVycGFyYW1ldGVyczogMiwNCgkJCX0NCgkJCWluZm8uZXhjZXB0aW9uaW5mb3JtYXRpb25bMF0gPSBncC5zaWdjb2RlMA0KCQkJaW5mby5leGNlcHRpb25pbmZvcm1hdGlvblsxXSA9IGdwLnNpZ2NvZGUxDQoJCX0gZWxzZSB7DQoJCQkvLyBCeSBkZWZhdWx0LCBhIGZhaWxpbmcgR28gYXBwbGljYXRpb24gZXhpdHMgd2l0aCBleGl0IGNvZGUgMi4NCgkJCS8vIFVzZSB0aGlzIHZhbHVlIHdoZW4gZ3AgZG9lcyBub3QgY29udGFpbiBleGNlcHRpb24gaW5mby4NCgkJCWluZm8gPSAmZXhjZXB0aW9ucmVjb3Jkew0KCQkJCWV4Y2VwdGlvbmNvZGU6IDIsDQoJCQl9DQoJCX0NCgl9DQoJY29uc3QgRkFJTF9GQVNUX0dFTkVSQVRFX0VYQ0VQVElPTl9BRERSRVNTID0gMHgxDQoJc3RkY2FsbDMoX1JhaXNlRmFpbEZhc3RFeGNlcHRpb24sIHVpbnRwdHIodW5zYWZlLlBvaW50ZXIoaW5mbykpLCB1aW50cHRyKHVuc2FmZS5Qb2ludGVyKHIpKSwgRkFJTF9GQVNUX0dFTkVSQVRFX0VYQ0VQVElPTl9BRERSRVNTKQ0KfQ0KDQovLyBnc2lnbmFsU3RhY2sgaXMgdW51c2VkIG9uIFdpbmRvd3MuDQp0eXBlIGdzaWduYWxTdGFjayBzdHJ1Y3R7fQ0K'

is_silent = False
is_skip_tests = False
is_extended_tests = False


# ====================================

def install_pip_package(package_name: str):
	command = f"pip show {package_name}"
	err_code, stdout, stderr = run_command(command, False, False)
	
	if err_code != 0:
		reply = ask_user(f'{package_name} python package is required for the installation, do you want me to install it?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'Cannot continue without {package_name}, please install it and try again')
		
		command = f"{sys.executable} -m pip install {package_name}"
		err_code, stdout, stderr = run_command(command, True, False)
		if err_code != 0:
			raise Exception(f"Failed installing {package_name} with the command {command}. Error code {err_code}. Output:\n{stdout}{stderr}")


def ask_user(input_text: str, default: str, valid_answers: list | None):
	global is_silent
	
	if is_silent:
		if default is None or default == '':
			raise Exception(f"internal error - missing default value in silent mode.\n{input_text}\n{valid_answers}")
		return default
	
	done = False
	
	msg = input_text
	if valid_answers is not None:
		msg += ' [' + '/'.join(valid_answers) + '] '
	if default is not None and default != '':
		msg += f'(default: {default}) '
	msg += ' - '
	
	answer = None
	
	while not done:
		answer = input(msg)
		answer = answer.strip()
		
		if answer == '' and default is not None and default != '':
			answer = default
		
		if valid_answers is not None:
			if answer.lower() not in [s.lower() for s in valid_answers]:
				print('Not a valid input.')
				continue
		
		done = True
	
	return answer


def is_windows():
	return platform.system() == 'Windows'


def is_ubuntu():
	if platform.system() != 'Linux':
		return False
	
	import distro
	return distro.name() == 'Ubuntu'


def is_path_string_valid(maybepath: str) -> bool:
	try:
		os.path.abspath(maybepath)
		return True
	except:
		return False


def get_install_dir(default_dir: str):
	install_dir = None
	
	# If METAFFI_HOME environment variable is set, return its value
	if "METAFFI_HOME" in os.environ:
		install_dir = os.environ["METAFFI_HOME"]
	
	# Otherwise, ask the user for the installation directory
	else:
		done = False
		while not done:
			# Ask the user for the input
			user_input = ask_user(f"Where to install. Notice due to issues in some languages, do not use whitespaces? ", default_dir, None)
			
			user_input = os.path.expanduser(os.path.expandvars(user_input))
			
			if ' ' in user_input:
				print('Installation directory mustn\'t contains whitespace')
				continue
			
			if not is_path_string_valid(user_input):
				print(f'Given path "{user_input}" is not valid')
				continue
			
			are_you_sure = ask_user(f'Are you sure you want to install to "{user_input}"?', 'y', ['y', 'n'])
			if are_you_sure.strip().lower() == 'n':
				continue
			
			install_dir = user_input
			done = True
	
	install_dir = os.path.abspath(install_dir)
	print('installing to ' + install_dir)
	return install_dir


# Define the function
def unpack_into_directory(base64_zip_file, target_directory):
	# Decode the base64 string to a byte array
	zip_data = base64.b64decode(base64_zip_file)
	
	zip_data = io.BytesIO(zip_data)  # to clear a pycharm warning
	
	# Create a zip file object from the byte array
	# Specify the compression type and level
	zf = zipfile.ZipFile(zip_data, "r", zipfile.ZIP_DEFLATED, compresslevel=9)
	
	# Check if the target directory exists
	if not os.path.exists(target_directory):
		# Create the target directory
		os.makedirs(target_directory)
	
	# Extract all the files from the zip file object to the target directory
	zf.extractall(target_directory)


refresh_env: typing.Callable


def run_command(command: str, raise_if_command_fail: bool = False, is_refresh_envvars: bool = True):
	global refresh_env
	
	if is_windows():
		print(f'{os.getcwd()}> {command}')
	else:
		print(f'{os.getcwd()}$ {command}')
	
	# create a process object with the command line
	if is_refresh_envvars:
		refresh_env()
	
	try:
		command_split = shlex.split(os.path.expanduser(os.path.expandvars(command)))
		output = subprocess.run(command_split, capture_output=True, text=True)
	except subprocess.CalledProcessError as e:
		if raise_if_command_fail:
			raise Exception(f'Failed running "{command}" with exit code {e.returncode}. Output:\n{str(e.stdout)}{str(e.stderr)}')
		
		# your code to handle the exception
		return e.returncode, str(e.stdout), str(e.stderr)
	except FileNotFoundError as e:
		if raise_if_command_fail:
			raise Exception(f'Failed running {command} with {e.strerror}.\nfile: {e.filename}')
		
		return f'Failed running {command} with {e.strerror}.\nfile: {e.filename}', '', ''
	
	all_stdout = output.stdout
	all_stderr = output.stderr
	
	# if the return code is not zero, raise an exception
	return output.returncode, str(all_stdout).strip(), str(all_stderr).strip()


def run_shell(command: str, raise_if_command_fail: bool = False):
	global refresh_env
	
	if is_windows():
		shell = 'cmd.exe'
		print(f'{os.getcwd()}> {command}')
	else:
		shell = "/bin/bash"
		print(f'{os.getcwd()}$ {command}')
	
	# create a process object with the command line
	refresh_env()
	try:
		command_split = shlex.split(os.path.expanduser(os.path.expandvars(command)))
		output = subprocess.run(command_split, executable=shell, capture_output=True, text=True, shell=True)
	except subprocess.CalledProcessError as e:
		
		if raise_if_command_fail:
			raise Exception(f'Failed running "{command}" with exit code {e.returncode}. Output:\n{str(e.stdout)}{str(e.stderr)}')
		
		# your code to handle the exception
		return e.returncode, str(e.stdout), str(e.stderr)
	except FileNotFoundError as e:
		if raise_if_command_fail:
			raise f'Failed running {command} with {e.strerror}.\nfile: {e.filename}'
		
		return 1, '', f'Failed running {command} with {e.strerror}.\nfile: {e.filename}'
	
	all_stdout = str(output.stdout).strip()
	all_stderr = str(output.stderr).strip()
	
	if raise_if_command_fail and output.returncode != 0:
		raise Exception(f'Failed running "{command}" with exit code {output.returncode}. Output:\n{all_stdout}{all_stderr}')
	
	# if the return code is not zero, raise an exception
	return output.returncode, all_stdout, all_stderr


def check_go_installed(install_go: typing.Callable):
	# try to run the go version command and capture the output
	
	exit_code, stdout, stderr = run_command("go version")
	
	if exit_code != 0:
		is_install_go = ask_user("Go was not detected, do you want me to install it for you", 'y', ['y', 'n'])
		if is_install_go == 'y':
			install_go()
			refresh_env()
		else:
			raise Exception(f"Go is not installed or not in the PATH. Make sure Go is installed and try again.")
	
	exit_code, stdout, stderr = run_command("go env GOROOT")
	# check if the command was successful
	if exit_code != 0:
		msg = f"An error occurred while running 'go env GOROOT' with: '{exit_code}'. The output is:\n"
		msg += stdout + '\n'
		msg += stderr
		raise Exception(msg)
	
	goroot = stdout
	if not goroot:
		raise Exception("GOROOT is not set. Please use the 'go env -w' command to set it and try again.")
	
	exit_code, stdout, stderr = run_command("go env CGO_ENABLED")
	
	if exit_code != 0:
		msg = f"An error occurred while running 'go env CGO_ENABLED'. The output is:\n"
		msg += stderr
		raise Exception(msg)
	
	# get the value of CGO_ENABLED from the output
	cgo_enabled = stdout
	# check if it is equal to "1"
	if cgo_enabled != "1":
		print(f"MetaFFI requires CGo is to be enabled")
		is_enable = ask_user("Do you want me to enable it?", 'y', ['y', 'n'])
		if is_enable.strip().lower() == 'y':
			output = subprocess.run("go env -w CGO_ENABLED=1".split(' '), capture_output=True, text=True)
			if output.returncode != 0:
				raise Exception(f'Failed to enable CGo. Command "go env -w CGO_ENABLED=1" failed:\n{output.stderr}')
			else:
				print('Enabled CGo')
		else:
			raise Exception('Cancelling installation')


def add_metaffi_home_to_cgo_cflags(install_dir: str, set_system_env_var: typing.Callable):
	exit_code, stdout, stderr = run_command("go env CGO_CFLAGS")
	
	if exit_code != 0:
		msg = f"An error occurred while running 'go env CGO_ENABLED'. The output is:\n"
		msg += stderr
		raise Exception(msg)
	
	# get the value of CGO_ENABLED from the output
	cgo_cflags = stdout
	
	if install_dir.lower() not in cgo_cflags.lower():
		# check if it is equal to "1"
		reply = ask_user(f'MetaFFI requires to add "{install_dir}" as an include directory to CGO_CFLAGS, do want me to do that for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'Set -I"{install_dir}" to CGO_CFLAGS and try again')
		
		# if cgo_cflags is not None and cgo_cflags != '':
		# 	new_cflags = f'"{cgo_cflags} -I{install_dir}"'
		# 	set_system_env_var('CGO_CFLAGS', f'{new_cflags}')
		# else:
		set_system_env_var('CGO_CFLAGS', f'-I{install_dir}')
		
		refresh_env()


def install_windows_gcc():
	# Check if gcc.exe exists
	gcc_path = shutil.which("gcc.exe")
	
	if gcc_path is not None:  # gcc installed
		return
	
	reply = ask_user('gcc, which is required for CGo, is not installed, do you want me to install it for you?', 'y', ['y', 'n'])
	if reply == 'n':
		raise Exception('Please install gcc and try again')
	
	url = "https://github.com/jmeubank/tdm-gcc/releases/download/v10.3.0-tdm64-2/tdm64-gcc-10.3.0-2.exe"
	
	# Download the exe file from the given URL
	print(f'Downloading and installing {url}')
	
	file_name = url.split("/")[-1]
	temp_dir = os.environ["TEMP"]
	file_path = os.path.join(temp_dir, file_name)
	
	# Use urllib.request.urlretrieve to download the file
	urllib.request.urlretrieve(url, file_path)
	
	exit_code, stdout, stderr = run_command(file_path.replace("\\", '/'))
	# os.remove(file_path)
	
	refresh_env()
	
	if exit_code != 0:
		raise Exception(f'Failed to install gcc. Error: {exit_code}.\nstdout: {stdout}\nstderr: {stderr}')
	
	return


def python_exe() -> str:
	if is_windows():
		return 'python'
	else:
		return shutil.which('python3.11')


# ========== unitests ==========

def get_exe_format(execname):
	if platform.system() == 'Windows':
		return f'{execname}.exe'
	else:
		return f'./{execname}'


def metaffi_go_guest(path: str, source: str):
	os.chdir(path)
	err_code, stdout, stderr = run_command(f'metaffi -c --idl {source} -g')
	if err_code != 0:
		raise Exception(f'Failed to build MetaFFI guest for {path}/{source}. Output:\n{stdout}{stderr}')


def run_python_file(path: str):
	with open("my_script.py") as f:
		code = f.read()
		exec(code)


def run_go_tests():
	def run(path: str, exec_name: str):
		os.chdir(path)
		
		err_code, stdout, stderr = run_command('go get -u')
		if err_code != 0:
			raise Exception(f'Failed "go get -u" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command('go build')
		if err_code != 0:
			raise Exception(f'Failed "go build" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command(exec_name)
		os.remove(exec_name)
		if err_code != 0:
			raise Exception(f'sanity failed in {path}.\n{stdout}{stderr}')
	
	print('Go --> Python3.11')
	run(os.environ['METAFFI_HOME'] + '/tests/go/sanity/python3', get_exe_format('python3'))
	
	print('Go --> OpenJDK')
	run(os.environ['METAFFI_HOME'] + '/tests/go/sanity/openjdk', get_exe_format('openjdk'))


def run_extended_go_tests():
	def run(path: str, exec_name: str):
		os.chdir(path)
		
		err_code, stdout, stderr = run_command('go get -u')
		if err_code != 0:
			raise Exception(f'Failed "go get -u" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command('go build')
		if err_code != 0:
			raise Exception(f'Failed "go build" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command(exec_name)
		os.remove(exec_name)
		if err_code != 0:
			raise Exception(f'sanity failed in {path}.\n{stdout}{stderr}')
	
	print('Go --> Python3.11 (BeautifulSoup)')
	install_pip_package('bs4')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/beautifulsoup/', get_exe_format('beautifulsoup'))
	
	print('Go --> Python3.11 (Collections)')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/collections/', get_exe_format('collections'))
	
	print('Go --> Python3.11 (Named, positional and variant arguments)')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/complex-primitives/', get_exe_format('complex-primitives'))
	
	print('Go --> Python3.11 (numpy)')
	install_pip_package('numpy')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/numpy/', get_exe_format('numpy'))
	
	print('Go --> Python3.11 (pandas)')
	install_pip_package('pandas')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/pandas/', get_exe_format('pandas'))
	
	print('Go --> OpenJDK')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/openjdk/log4j/', get_exe_format('log4j'))


def run_openjdk_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		metaffi_home = os.environ['METAFFI_HOME'] + '/'
		openjdk_api = metaffi_home + 'metaffi.api.jar'
		bridge = metaffi_home + 'xllr.openjdk.bridge.jar'
		junit = metaffi_home + '/tests/openjdk/junit-platform-console-standalone-1.10.1.jar'
		hamcrest = metaffi_home + '/tests/openjdk/hamcrest-core-1.3.jar'
		
		error_code, stdout, stderr = run_command(f'javac -cp "{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{junit}{os.pathsep}{hamcrest}" {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed compiling openjdk test:\n{stdout}{stderr}')
		
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'java -jar "{junit}" -cp ".{os.pathsep}{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{hamcrest}" -d. --select-class {test_file} --details=Verbose --fail-if-no-tests --disable-banner')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code} openjdk sanity test:\n{stdout}{stderr}')
	
	print('OpenJDK --> Python3.11')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/sanity/', 'APITestPython3.java')
	
	print('OpenJDK --> Go')
	metaffi_go_guest(os.environ['METAFFI_HOME'] + '/tests/openjdk/sanity/go', 'TestRuntime.go')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/sanity/', 'APITestGo.java')


def run_extended_openjdk_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		metaffi_home = os.environ['METAFFI_HOME'] + '/'
		openjdk_api = metaffi_home + 'metaffi.api.jar'
		bridge = metaffi_home + 'xllr.openjdk.bridge.jar'
		junit = metaffi_home + '/tests/openjdk/junit-platform-console-standalone-1.10.1.jar'
		hamcrest = metaffi_home + '/tests/openjdk/hamcrest-core-1.3.jar'
		
		error_code, stdout, stderr = run_command(f'javac -cp "{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{junit}{os.pathsep}{hamcrest}" {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed compiling openjdk test:\n{stdout}{stderr}')
		
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'java -jar "{junit}" -cp ".{os.pathsep}{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{hamcrest}" -d. --select-class {test_file} --details=Verbose --fail-if-no-tests --disable-banner')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code} openjdk sanity test:\n{stdout}{stderr}')
	
	print('OpenJDK --> Python3.11 (BeautifulSoup)')
	install_pip_package('bs4')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/beautifulsoup/', 'BeautifulSoupTest.java')
	
	print('OpenJDK --> Python3.11 (Collections)')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/collections/', 'CollectionsTest.java')
	
	print('OpenJDK --> Python3.11 (Named, positional and variant arguments)')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/complex-primitives/', 'ComplexPrimitivesTest.java')
	
	print('OpenJDK --> Python3.11 (numpy)')
	install_pip_package('numpy')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/numpy/', 'NumpyTest.java')
	
	print('OpenJDK --> Python3.11 (pandas)')
	install_pip_package('pandas')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/pandas/', 'PandasTest.java')
	
	print('OpenJDK --> Go (gomcache)')
	run_python_file(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/go/gomcache/build_metaffi.py')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/go/gomcache/', 'GoMCacheTest.java')


def run_python_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'{python_exe()} -m unittest {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code}:\n{stdout}{stderr}')
	
	print('Python3.11 --> OpenJDK')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/sanity/openjdk/', 'openjdk_test.py')
	
	print('Python3.11 --> Go')
	metaffi_go_guest(os.environ['METAFFI_HOME'] + '/tests/python3/sanity/go/', 'TestRuntime.go')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/sanity/go/', 'go_test.py')


def run_extended_python_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'{python_exe()} -m unittest {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code}:\n{stdout}{stderr}')
	
	print('Python3.11 --> OpenJDK (log4j)')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/extended/openjdk/log4j/', 'log4j_test.py')
	
	print('Python3.11 --> Go (gomcache)')
	run_python_file(os.environ['METAFFI_HOME'] + '/tests/python3/extended/go/gomcache/build_metaffi.py')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/extended/go/gomcache/', 'gomcache_test.py')


def run_sanity_tests():
	run_python_tests()
	run_openjdk_tests()
	run_go_tests()


def run_extended_tests():
	run_extended_python_tests()
	run_extended_openjdk_tests()
	run_extended_go_tests()


def install_python3_api():
	answer = ask_user('Do you want to install MetaFFI Python3 API?', 'y', ['y', 'n'])
	
	if answer.strip() == 'y':
		err_code, stdout, stderr = run_command(f"{python_exe()} -m pip install -i https://test.pypi.org/simple/ metaffi-api")
		if err_code != 0:
			raise Exception(f'Failed to install metaffi-api python package (error code: {err_code}):\n{stderr}{stdout}')


def install_apis():
	install_python3_api()


# ========== windows ===========


def set_windows_system_environment_variable(name: str, val: str):
	import winreg
	
	# already set correctly, no need to set
	if name in os.environ and val == os.environ[name]:
		return
	
	# Open the registry key for system environment variables
	key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, winreg.KEY_ALL_ACCESS)
	# Set the value of the environment variable
	# Use REG_EXPAND_SZ as the type to allow references to other variables
	winreg.SetValueEx(key, name, 0, winreg.REG_EXPAND_SZ, val)
	# Close the key
	winreg.CloseKey(key)
	# Notify the system that the environment variables have changed
	# Use HWND_BROADCAST and WM_SETTINGCHANGE messages
	ctypes.windll.user32.SendMessageW(0xFFFF, 0x001A, 0, "Environment")
	
	print(f'Setting system-wide environment variable {name} to {val}')


def set_windows_user_environment_variable(name: str, val: str):
	import winreg
	
	# open the registry key for the current user's environment variables
	key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", access=winreg.KEY_WRITE)
	
	# set the value of the environment variable in the registry
	winreg.SetValueEx(key, name, 0, winreg.REG_SZ, val)
	
	# close the registry key
	winreg.CloseKey(key)
	
	# broadcast a message to all windows that the environment has changed
	HWND_BROADCAST = 0xFFFF
	WM_SETTINGCHANGE = 0x1A
	SMTO_ABORTIFHUNG = 0x0002
	result = ctypes.c_long()
	SendMessageTimeoutW = ctypes.windll.user32.SendMessageTimeoutW
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, u"Environment", SMTO_ABORTIFHUNG, 5000, ctypes.byref(result))


# Define the function
def add_to_path_environment_variable(path):
	import winreg
	
	# Open the registry key for system environment variables
	key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, winreg.KEY_ALL_ACCESS)
	
	# Get the current value of the Path variable
	current_path = winreg.QueryValueEx(key, "Path")[0]
	
	# Check if the given path is already in the Path variable
	if not any(os.path.samefile(path, os.path.expanduser(os.path.expandvars(p))) for p in current_path.split(";")):
		# Append the given path to the Path variable
		new_path = current_path + ";" + path
		
		# Set the new value of the Path variable
		# Use REG_EXPAND_SZ as the type to allow references to other variables
		winreg.SetValueEx(key, "Path", 0, winreg.REG_EXPAND_SZ, new_path)
		
		# Notify the system that the environment variables have changed
		# Use HWND_BROADCAST and WM_SETTINGCHANGE messages
		ctypes.windll.user32.SendMessageW(0xFFFF, 0x001A, 0, "Environment")
		
		print(f'Adding {path} to PATH environment variable')


def get_dll_version(filename):
	import ctypes
	# Define the VS_FIXEDFILEINFO structure
	class VS_FIXEDFILEINFO(ctypes.Structure):
		_fields_ = [
			("dwSignature", ctypes.c_ulong),
			("dwStrucVersion", ctypes.c_ulong),
			("dwFileVersionMS", ctypes.c_ulong),
			("dwFileVersionLS", ctypes.c_ulong),
			("dwProductVersionMS", ctypes.c_ulong),
			("dwProductVersionLS", ctypes.c_ulong),
			("dwFileFlagsMask", ctypes.c_ulong),
			("dwFileFlags", ctypes.c_ulong),
			("dwFileOS", ctypes.c_ulong),
			("dwFileType", ctypes.c_ulong),
			("dwFileSubtype", ctypes.c_ulong),
			("dwFileDateMS", ctypes.c_ulong),
			("dwFileDateLS", ctypes.c_ulong),
		]
	
	# Load the version.dll library
	version = ctypes.windll.version
	# Get the size of the file version info
	size = version.GetFileVersionInfoSizeA(filename.encode('ascii'), None)
	if size == 0:
		# Get the last error code
		err_code = ctypes.get_last_error()
		# Get the error message
		err_msg = ctypes.FormatError(err_code)
		# Raise a WindowsError exception
		raise WindowsError(err_code, err_msg)
	# Allocate a buffer for the file version info
	buffer = ctypes.create_string_buffer(size)
	# Get the file version info
	res = version.GetFileVersionInfoA(filename.encode('ascii'), None, size, buffer)
	if res == 0:
		# Get the last error code
		err_code = ctypes.get_last_error()
		# Get the error message
		err_msg = ctypes.FormatError(err_code)
		# Raise a WindowsError exception
		raise WindowsError(err_code, err_msg)
	# Get a pointer to the VS_FIXEDFILEINFO structure
	ffi = ctypes.POINTER(VS_FIXEDFILEINFO)()
	pLen = ctypes.c_uint()
	# Query the value of the VS_FIXEDFILEINFO structure
	res = version.VerQueryValueA(buffer, "\\", ctypes.byref(ffi), ctypes.byref(pLen))
	if res == 0:
		# Get the last error code
		err_code = ctypes.get_last_error()
		# Get the error message
		err_msg = ctypes.FormatError(err_code)
		# Raise a WindowsError exception
		raise WindowsError(err_code, err_msg)
	# Extract the file version from the VS_FIXEDFILEINFO structure
	ms = ffi.contents.dwFileVersionMS
	ls = ffi.contents.dwFileVersionLS
	return f'{ms >> 16}.{ms & 0xFFFF}.{ls >> 16}.{ls & 0xFFFF}'


def install_windows_python():
	return_code, all_stdout, all_stderr = run_command('winget install winget install -e --silent --accept-source-agreements --accept-package-agreements --id Python.Python.3.11')
	if return_code != 0:
		if 'Found an existing package already installed.' not in all_stdout and 'Found an existing package already installed.' not in all_stderr:
			raise Exception('Failed to install Python 3.11')


def install_windows_openjdk():
	return_code, all_stdout, all_stderr = run_command('winget install -e --silent --accept-source-agreements --accept-package-agreements --id Microsoft.OpenJDK.11')
	if return_code != 0:
		if 'Found an existing package already installed.' not in all_stdout and 'Found an existing package already installed.' not in all_stderr:
			raise Exception('Failed to install OpenJDK')


def install_windows_go():
	refresh_env()
	return_code, all_stdout, all_stderr = run_command('winget install -e --silent --accept-source-agreements --accept-package-agreements --id GoLang.Go')
	print(os.environ['PATH'])
	if return_code != 0:
		if 'Found an existing package already installed.' not in all_stdout and 'Found an existing package already installed.' not in all_stderr:
			raise Exception(f'Failed to install Go. returned with: {return_code}.\nstdout: {all_stdout}\nstderr: {all_stderr}')


def check_python_windows_installed(version: str):
	from ctypes import wintypes
	
	# split the version number into major, minor, and micro parts
	major, minor = map(int, version.split("."))
	
	# construct the DLL file name based on the version number
	dll_name = f"python{major}{minor}.dll"
	
	# try to load the DLL file using ctypes
	try:
		dll = ctypes.WinDLL(dll_name)
	except OSError:
		is_install_python = input("Python3.11 was not detected, do you want me to install it for you? [y/n] (default: n) ")
		is_install_python = is_install_python.strip().lower()
		if is_install_python == 'y':
			install_windows_python()
		else:
			# if the DLL file cannot be loaded, print an error message
			raise Exception(f"{dll_name} cannot be loaded. Please check your Python {version} is installed, and in PATH environment variable")
	
	# Define the GetModuleFileName function
	kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
	kernel32.GetModuleFileNameW.restype = wintypes.DWORD
	kernel32.GetModuleFileNameW.argtypes = (wintypes.HMODULE, wintypes.LPWSTR, wintypes.DWORD)
	# Call the GetModuleFileName function with the DLL handle
	buffer = ctypes.create_unicode_buffer(wintypes.MAX_PATH)
	length = kernel32.GetModuleFileNameW(dll._handle, buffer, len(buffer))
	if length == 0:
		raise ctypes.WinError(ctypes.get_last_error())
	
	dll_version = get_dll_version(buffer.value)
	
	# compare the DLL version with the expected version
	if not dll_version.startswith(version):
		# if they don't match, print an error message
		raise Exception(f"Python was found {dll_name}, but with version {dll_version}, which does not match the expected version {version}. Please install the supported Python version.")


def check_windows_pythonhome(version: str):
	from ctypes import util
	
	major, minor = map(int, version.split("."))
	
	# construct the DLL file name based on the Python version
	dll_name = f"python{major}{minor}.dll"
	
	# try to get the full path of the DLL file using ctypes
	try:
		dll_path = util.find_library(dll_name)
	except OSError:
		raise Exception(f"{dll_name} cannot be found. Please check your Python installation.")
	
	if "PYTHONHOME" in os.environ:
		python_home_val = os.path.expandvars(os.environ['PYTHONHOME'])
		python311_path = os.path.dirname(dll_path)
		if os.path.samefile(python_home_val, python311_path):
			# PYTHONHOME is set
			return
		else:
			raise Exception(f"PYTHONHOME exists and set to {python_home_val}, while {python311_path} is expected.")
	
	# set PYTHONHOME
	dll_dir = os.path.dirname(dll_path)
	
	reply = ask_user(f"PYTHONHOME environment variable is not set. Do you want me to set it to {dll_dir}?", 'y', ['y', 'n'])
	reply = reply.strip()
	if reply.lower() == '' or reply.lower() == 'y':
		print('Setting PYTHONHOME')
		set_windows_user_environment_variable('PYTHONHOME', dll_dir)
	else:
		raise Exception(f'Set PYTHONHOME environment variable to {dll_dir} and try again')


def refresh_windows_env():
	import winreg
	
	# system environment variables
	key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", access=winreg.KEY_READ)
	
	# get the number of values in the key
	num_values = winreg.QueryInfoKey(key)[1]
	
	# loop through the values
	for i in range(num_values):
		# get the name and value of the environment variable
		name, value, _ = winreg.EnumValue(key, i)
		# update the os.environ dictionary with the new value
		value = os.path.expandvars(os.path.expanduser(value))
		os.environ[name] = value
	
	# close the registry key
	winreg.CloseKey(key)
	
	# broadcast a message to all windows that the environment has changed
	HWND_BROADCAST = 0xFFFF
	WM_SETTINGCHANGE = 0x1A
	SMTO_ABORTIFHUNG = 0x0002
	result = ctypes.c_long()
	SendMessageTimeoutW = ctypes.windll.user32.SendMessageTimeoutW
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, u"Environment", SMTO_ABORTIFHUNG, 5000, ctypes.byref(result))
	
	# user environment variables
	# open the registry key for the current user's environment variables
	key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", access=winreg.KEY_READ)
	
	# get the number of values in the key
	num_values = winreg.QueryInfoKey(key)[1]
	
	# loop through the values
	for i in range(num_values):
		# get the name and value of the environment variable
		name, value, _ = winreg.EnumValue(key, i)
		# update the os.environ dictionary with the new value
		value = os.path.expandvars(os.path.expanduser(value))
		if name in os.environ and name.lower() == 'path':
			os.environ[name] += ';' + value
		else:
			os.environ[name] = value
	
	# close the registry key
	winreg.CloseKey(key)
	
	# broadcast a message to all windows that the environment has changed
	HWND_BROADCAST = 0xFFFF
	WM_SETTINGCHANGE = 0x1A
	SMTO_ABORTIFHUNG = 0x0002
	result = ctypes.c_long()
	SendMessageTimeoutW = ctypes.windll.user32.SendMessageTimeoutW
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, u"Environment", SMTO_ABORTIFHUNG, 5000, ctypes.byref(result))


def refresh_ubuntu_env():
	from dotenv import load_dotenv
	
	load_dotenv("/etc/environment")
	load_dotenv("~/.profile")


if is_windows():
	refresh_env = refresh_windows_env
elif is_ubuntu():
	refresh_env = refresh_ubuntu_env


def check_windows_java_jni_installed(version: str):
	java_path = shutil.which("java.exe")
	if java_path is None:
		is_install_java = ask_user("Java was not detected, do you want me to install it for you?", 'y', ['y', 'n'])
		if is_install_java == 'y':
			install_windows_openjdk()
			refresh_windows_env()
		else:
			raise Exception('JAVA_HOME is not set. Make sure JVM is installed and JAVA_HOME environment variable is set and try again.')
	
	# try to get the value of JAVA_HOME environment variable
	java_home = os.environ.get("JAVA_HOME")
	java_home = os.path.expanduser(os.path.expandvars(java_home))
	
	if not java_home:
		raise Exception('Java was found, but JAVA_HOME is not set, check your JVM installation.')
	
	# construct the path to jvm.dll
	jvm_path = os.path.join(java_home, "bin", "server", "jvm.dll")
	
	if not os.path.exists(jvm_path):
		raise Exception(f'Cannot find jvm.dll for JNI at {jvm_path}. Please check your installation to include JNI and try again.')
	
	# try to load jvm.dll
	add_to_path_environment_variable(os.path.dirname(java_home))
	add_to_path_environment_variable(os.path.dirname(java_home + '\\bin\\server\\'))
	refresh_env()
	
	try:
		dll = ctypes.cdll.LoadLibrary(jvm_path)
	except OSError as exp:
		raise Exception(f"Failed to load {jvm_path}. {exp.strerror}. Please check {os.path.dirname(jvm_path)} is in PATH environment variable and try again.")
	
	# Check it is version 11
	command = "java -version"
	# run the command and capture the output
	error_code, stdout, stderr = run_command(command)
	
	# check if the output contains the word JNI
	if f'version "{version}' not in stdout and f'version "{version}' not in stderr:
		raise Exception(f'MetaFFI currently supports JVM Version {version} is supported, while the installed JVM is:\n{stderr}\nInstall JVM version {version} and try again.')


def patch_windows_go():
	global windows_patched_signal_file
	
	exit_code, stdout, stderr = run_command("go env GOROOT")
	if exit_code != 0:
		raise Exception(f'Failed to run go env GOROOT.\nexit code: {exit_code}\nstdout: {stdout}\nstderr:{stderr}\n')
	
	goroot = stdout.strip()
	signal_windows_path = f'{goroot}\\src\\runtime\\signal_windows.go'
	signal_windows_backup_path = f'{goroot}\\src\\runtime\\signal_windows.go.bck'
	
	with open(signal_windows_path, "r") as f:
		data = f.read()
		if data == base64.b64decode(windows_patched_signal_file):
			# already patched
			return
	
	print('\nGo application in windows is unable embed JVM code, due to an existing issue (https://github.com/golang/go/issues/58542).')
	print(f'MetaFFI installer can patch the file {signal_windows_path} to fix the issue (in Go 1.21.x), until a release of an official fix.')
	print(f'Patching the file creates a backup of the original file at {signal_windows_backup_path}. In case of any issues, recover the original file from the backup')
	print('Notice, updating Go will require re-patching the file.')
	is_patch = ask_user('Do you want me to patch the file and enable loading JVM within Go applications?', 'y', ['y', 'n'])
	if is_patch.strip().lower() != 'y':
		print('Skipping patching Go. Cannot load JVM in Go applications')
		return
	
	print('patching the file')
	shutil.copy(f'{signal_windows_path}', f'{signal_windows_backup_path}')
	with open(signal_windows_path, "wb") as f:
		# write the decoded data to the file
		f.write(base64.b64decode(windows_patched_signal_file))


def check_windows_prerequisites():
	print('checking prerequisites...')
	
	# python
	check_python_windows_installed('3.11')
	check_windows_pythonhome('3.11')
	
	# openjdk
	check_windows_java_jni_installed('11')
	
	# go
	check_go_installed(install_windows_go)
	patch_windows_go()
	
	# gcc
	install_windows_gcc()


def install_windows():
	global windows_x64_zip
	
	# verify running as admin
	is_admin = ctypes.windll.shell32.IsUserAnAdmin() != 0
	if not is_admin:
		raise Exception('User must have admin privileges')
	
	refresh_windows_env()  # refresh environment variables, in case the environment is not up-to-date
	
	check_windows_prerequisites()
	
	print()
	print('==== Starting installation ====')
	print()
	
	# get install dir
	install_dir = get_install_dir("c:\\MetaFFI\\")
	
	# unpack zip into install dir
	unpack_into_directory(windows_x64_zip, install_dir)
	
	# setting METAFFI_HOME environment variable
	set_windows_system_environment_variable("METAFFI_HOME", install_dir)
	
	# add install_dir and install_dir\bin to PATH
	add_to_path_environment_variable(install_dir)
	add_to_path_environment_variable(install_dir + '\\bin\\')
	
	add_metaffi_home_to_cgo_cflags(install_dir, set_windows_system_environment_variable)
	
	install_apis()
	
	print()
	print('==== Running Sanity Tests ====')
	print()
	
	run_sanity_tests()
	
	print('Done')


# -------------------------------


def get_ubuntu_environment_variable(file: str, name: str) -> str | None:
	# Execute the command and capture the output
	err_code, stdout, stderr = run_shell(f"grep -q '{name}=' {file}")
	
	if err_code != 0:
		return None
	
	err_code, stdout, stderr = run_shell(f"grep '{name}=' {file} | cut -d '=' -f 2")
	
	# Check the return code and get the value
	if err_code != 0:
		raise Exception(f'Failed to get the value of the environment variable in {file}: {stdout}{stderr}')
	
	return stdout.strip()


def get_ubuntu_user_environment_variable(name: str) -> str | None:
	return get_ubuntu_environment_variable(name, '~/.profile')


def get_ubuntu_machine_environment_variable(name: str) -> str | None:
	return get_ubuntu_environment_variable(name, '/etc/environment')


def set_ubuntu_environment_variable(file: str, name: str, value: str):
	existing_val = get_ubuntu_environment_variable(name, file)
	
	if existing_val is not None:  # env var exists
		if existing_val == value:
			return
		
		update_value = ask_user(f'Existing {name} is {existing_val}, do you want me to update it to {value} in {file}?', 'y', ['y', 'n'])
		if update_value == 'n':
			raise Exception(f'{name} must be {value} in order to continue. Make sure {name} points to the required python and try again')
		
		print(f'Updating environment variable {name}={value} in {file}')
		
		# update value
		err_code, stdout, stderr = run_shell(f"sed -i 's/{name}={existing_val}/{name}={value}/g' {file}")
		if err_code != 0:
			raise Exception(f'Failed to update {name} environment variable in {file}: {stdout}{stderr}')
		return
	
	else:  # var doesn't exist
		print(f'Adding environment variable {name}={value} in {file}')
		err_code, stdout, stderr = run_shell(f"echo 'export {name}={value}' >> {file}")
		if err_code != 0:
			raise Exception(f"Failed to add {name} environment variable to {file}: {stdout}{stderr}")
	
	refresh_env()


def set_ubuntu_user_environment_variable(name: str, value: str):
	set_ubuntu_environment_variable('~/.profile', name, value)


def set_ubuntu_machine_environment_variable(name: str, value: str):
	set_ubuntu_system_environment_variable(name, value)


def check_python_ubuntu_installed(version: str):
	# split the version number into major, minor, and micro parts
	major, minor = map(int, version.split("."))
	
	# construct the executable file name based on the version number
	exe_name = f"python{major}.{minor}"
	
	# try to run the executable file using subprocess
	exit_code, stdout, stderr = run_command(f'{exe_name} --version')
	
	if exit_code != 0 or not stdout.strip().startswith(f"Python {version}"):
		reply = ask_user(f'Python {exe_name} is not installed, do you want me to install it for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f"{exe_name} cannot be found. Please check your Python {version} is installed, and in PATH environment variable")
		
		# install python
		exit_code, stdout, stderr = run_command(f'apt install {exe_name} -y')
		if exit_code != 0:
			raise Exception(f'Failed to install {exe_name}. Output\n{stdout}{stderr}')


def check_ubuntu_pythonhome(version: str):
	major, minor = map(int, version.split("."))
	
	# construct the executable file name based on the Python version
	exe_name = f"python{major}.{minor}"
	
	# try to get the full path of the executable file using os
	try:
		exe_path = shutil.which(exe_name)
	except OSError:
		raise Exception(f"{exe_name} cannot be found. Please check your Python installation.")
	
	if "PYTHONHOME" in os.environ:
		python_home_val = os.path.expanduser(os.environ['PYTHONHOME'])
		maybe_python311_path = os.path.dirname(exe_path)
		if os.path.samefile(python_home_val, maybe_python311_path):
			# PYTHONHOME is set
			return
		else:
			raise Exception(f"PYTHONHOME exists, but set to {python_home_val}, while {exe_path} is expected.")
	
	# set PYTHONHOME
	exe_dir = os.path.dirname(exe_path)
	
	reply = ask_user(f"PYTHONHOME environment variable is not set. Do you want me to set it to {exe_dir}?", 'y', ['y', 'n'])
	if reply.lower() == 'y':
		print('Setting PYTHONHOME')
		set_ubuntu_user_environment_variable('PYTHONHOME', exe_dir)
	else:
		raise Exception(f'Set PYTHONHOME environment variable to {exe_dir} and try again')


def get_java_home(version: str) -> str | None:
	output = os.popen("update-java-alternatives -l").read()
	lines = output.split("\n")
	
	for line in lines:
		# use regular expression to extract the name, priority and path of the alternative
		match = re.search(r"(\S+)\s+(\d+)\s+(\S+)", line)
		if not match:
			continue
		
		# get the name of the alternative
		name = match.group(1)
		
		# check if the name contains the given version
		if version in name:
			path = match.group(3)
			return path
	
	return None


def check_ubuntu_java_jni_installed(version: str):
	# Execute the command and get the output
	err_code, stdout, stderr = run_command(f"java -version {version}")
	
	# Check the return code and print the result
	if err_code != 0:
		
		reply = ask_user(f'JVM {version} is not installed. Do you want me to install it for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'JVM {version} must be installed in order to continue the installation. Install JVM {version} and try again.')
		
		# Execute the command and get the output
		err_code, stdout, stderr = run_command(f"apt install -y openjdk-{version}-jdk")
		refresh_env()
		
		if err_code != 0:
			raise Exception(f"Failed to install openjdk-{version}-jdk with error code {err_code}. Output:\n{stdout}{stderr}")
	
	java_location = get_java_home(version)
	
	if "JAVA_HOME" in os.environ and not os.environ["JAVA_HOME"] == java_location:
		cur_java_home = os.environ["JAVA_HOME"]
		raise Exception(f"JAVA_HOME is already set, but it is set to {cur_java_home} and not to {java_location}. Update the environment variable and try again")
	
	# set JAVA_HOME
	set_ubuntu_machine_environment_variable('JAVA_HOME', java_location)
	
	# Try to load libjvm.so
	loaded_jni = False
	try:
		ctypes.cdll.LoadLibrary('libjvm.so')
		loaded_jni = True
	except:
		pass
	
	if not loaded_jni:
		
		# make sure libjvm.so exists
		jni_path = f'{java_location}/lib/server/libjvm.so'
		if not os.path.exists(jni_path):
			raise Exception(f'{jni_path} not found. Please check your JVM installation and try again.')
		
		reply = ask_user(f'failed to load {jni_path}. Do you want me to fix it by placing a symbolic link of {jni_path} to /usr/lib/?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'Please make sure libjvm.so can be loaded, and try again')
		
		# Execute the command and get the output
		err_code, stdout, stderr = run_command(f"ln -s {jni_path} /usr/lib/libjvm.so")
		if err_code != 0:
			raise Exception(f'Failed to create a symbolic link from {jni_path} to /usr/lib/libjvm.so. Output:\n{stdout}{stderr}')
		
		try:
			ctypes.cdll.LoadLibrary('libjvm.so')
		except Exception as e:
			raise Exception(f'Although libjvm.so has been linked to /usr/lib/, I still cannot load it.\nError: {str(e)}')


def verify_pip_packages_installed():
	install_pip_package('python-dotenv')


def install_ubuntu_go():
	import requests
	
	# Define the command to check if Go exists
	command = "which go"
	
	# Execute the command and get the output
	err_code, stdout, stderr = run_command(command)
	
	# Check the return code and print the result
	if err_code == 0:
		
		# make sure go has supported version
		
		# Define the command to check the go version
		command = "go version"
		
		# Execute the command and get the output
		err_code, stdout, stderr = run_command(command)
		
		# Check the return code and get the value
		if err_code != 0:
			raise Exception(f"Although Go is installed, failed the command {command} with error code {err_code}. Output:\n{stdout}{stderr}")
		
		go_version = stdout.split()[2].replace("go", "")
		
		# Compare the go version with the minimum required version
		if go_version < "1.21.0":
			# The go version is below 1.21.0
			# Raise an exception with the error message
			raise Exception(f"Go version {go_version} is not supported. Please install at least Go 1.21.0 and try again.")
		
		# make sure CGO_ENABLED=1
		err_code, stdout, stderr = run_command("go env -w CGO_ENABLED=1")
		if err_code != 0:
			raise Exception(f"Failed settings CGO_ENABLED=1. error code {err_code}. Output:\n{stdout}{stderr}")
	
	else:  # Go does not exist
		
		# install go 1.21.5
		dl_link = f"https://go.dev/dl/go1.21.5.linux-amd64.tar.gz"
		
		with tempfile.TemporaryDirectory() as tempdir:
			# Get the file name from the link
			filename = dl_link.split("/")[-1]
			filepath = os.path.join(tempdir, filename)
			
			urllib.request.urlretrieve(dl_link, filepath)
			
			# Extract the file to "/usr/local"
			command = f"tar -C /usr/local -xzf {filepath}"
			
			# Execute the command and get the output
			err_code, stdout, stderr = run_command(command)
			
			# Check the return code and print the result
			if err_code != 0:
				raise Exception(f"Failed extracting downloaded Go using the command {command}. Failed with error code {err_code}. Output:\n{stdout}{stderr}")
		
		run_command(f'ln -s /usr/local/go/bin/go /usr/bin/go', True)
		
		# Update the PATH environment variable to include /usr/local/go/bin
		command = f"echo 'export PATH=$PATH:/usr/local/go/bin' | tee -a /etc/environment"
		err_code, stdout, stderr = run_command(command)
		if err_code != 0:
			raise Exception(f"Failed to add Go to PATH using the command {command}, which failed with error code {err_code}. Output:\n{stdout}{stderr}")
		
		# set environment variable CGO_ENABLED=1
		set_ubuntu_system_environment_variable('CGO_ENABLED', '1')


def check_ubuntu_gcc_installed():
	err_code, stdout, stderr = run_command("gcc --version")
	if err_code != 0:
		
		reply = ask_user('GCC is not installed, do you want me to install it for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception("Please install GCC and try again.")
		
		command = "apt install gcc -y"
		err_code, stdout, stderr = run_command(command)
		
		if err_code != 0:
			raise Exception(f"Failed to install GCC using the {command}. error code {err_code}. Output:\n{stdout}{stderr}")


def check_pip_installed():
	exit_code, stdout, stderr = run_command('add-apt-repository ppa:deadsnakes/ppa -y', False, False)
	if exit_code != 0:
		raise Exception(f'Failed to add ppa:deadsnakes/paa. Output\n{stdout}{stderr}')
	
	err_code, stdout, stderr = run_command(f'{sys.executable} -m pip --version', False, False)
	if err_code != 0:
		err_code, stdout, stderr = run_command(f'apt install python3-pip -y', False, False)
		if err_code != 0:
			raise Exception(f'Failed to install pip with error code: {err_code}. Output:\n{stdout}{stderr}')


def check_ubuntu_prerequisites():
	print('checking prerequisites...')
	
	check_pip_installed()
	verify_pip_packages_installed()
	
	refresh_env()  # refresh environment variables, in case the environment is not up-to-date
	
	# python
	check_python_ubuntu_installed('3.11')
	check_ubuntu_pythonhome('3.11')
	
	# openjdk
	check_ubuntu_java_jni_installed('11')
	
	# go
	check_go_installed(install_ubuntu_go)
	
	# gcc
	check_ubuntu_gcc_installed()


def set_ubuntu_system_environment_variable(name: str, value: str):
	# construct the file name for the environment file
	env_file = "/etc/environment"
	
	# read the existing lines from the environment file
	with open(env_file, "r") as f:
		lines = f.readlines()
	
	# check if the environment variable already exists in the environment file
	for i, line in enumerate(lines):
		if line.startswith(f"{name}="):
			# get the current value of the environment variable
			current_value = line.split("=")[1].strip()
			if current_value == value:
				# if the value is the same, do nothing
				return
			else:
				# if the value is different, replace the line with the new value
				lines[i] = f"{name}={value}\n"
				break
	else:
		# if the environment variable does not exist, append a new line with the name and value
		lines.append(f"{name}={value}\n")
	
	# write the modified lines back to the environment file
	with open(env_file, "w") as f:
		f.writelines(lines)
	
	refresh_env()


def make_metaffi_available_globally(install_dir: str):
	run_command(f'chmod u+x {install_dir}/metaffi', True)
	run_command(f'ln -s {install_dir}/metaffi /usr/bin/metaffi', True)


def install_ubuntu():
	global ubuntu_x64_zip
	
	# verify running as admin
	is_admin = os.getuid() == 0
	if not is_admin:
		raise Exception('Installer must run as sudo')
	
	check_ubuntu_prerequisites()
	
	print()
	print('==== Starting installation ====')
	print()
	
	# get install dir
	install_dir = get_install_dir("/usr/local/metaffi/")
	
	# unpack zip into install dir
	unpack_into_directory(ubuntu_x64_zip, install_dir)
	
	make_metaffi_available_globally(install_dir)
	
	# setting METAFFI_HOME environment variable
	set_ubuntu_system_environment_variable("METAFFI_HOME", install_dir)
	
	add_metaffi_home_to_cgo_cflags(install_dir, set_ubuntu_machine_environment_variable)
	
	refresh_env()
	install_apis()
	
	if 'METAFFI_HOME' not in os.environ:
		raise Exception('METAFFI_HOME not in os.environ')
	
	print()
	print('==== Running Sanity Tests ====')
	print()
	
	global is_skip_tests
	global is_extended_tests
	
	if not is_skip_tests:
		run_sanity_tests()
		
		if is_extended_tests:
			run_extended_tests()
	
	print('Done')


# -------------------------------


def set_is_silent():
	global is_silent
	global is_skip_tests
	global is_extended_tests
	
	for arg in sys.argv:
		arg = arg.lower()
		
		if arg == '-h' or arg == '--help' or arg == '/?' or arg == '/h':
			print('MetaFFI Installer')
			print('-s - silent mode (using defaults)')
			print('--skip-sanity - skips all tests after installation')
			print('--include-extended-tests - runs extended tests after installation (downloads several 3rd party libraries for tests)')
		
		if arg == "/s" or arg == "-s":
			is_silent = True
		
		if arg == '--skip-sanity':
			is_skip_tests = True
		
		if arg == '--include-extended-tests':
			is_extended_tests = True


def main():
	set_is_silent()
	
	try:
		if platform.system() == 'Windows':
			install_windows()
		elif platform.system() == 'Linux':
			import distro
			if distro.name() == 'Ubuntu':
				install_ubuntu()
			else:
				print("Installer supports Ubuntu distribution, not {}".format(distro.name()), file=sys.stderr)
				exit(1)
		else:
			print("Installer doesn't support platform {}".format(platform.system()), file=sys.stderr)
			exit(1)
	except Exception as exp:
		traceback.print_exc()
		exit(2)
	
	print('\nInstallation Complete')


if __name__ == '__main__':
	main()
