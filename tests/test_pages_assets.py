from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import urlsplit


ROOT = Path(__file__).resolve().parents[1]
SITE = ROOT / "html"


class Assets(HTMLParser):
    def __init__(self):
        super().__init__()
        self.urls = []

    def handle_starttag(self, tag, attrs):
        attributes = dict(attrs)
        if tag == "script" and attributes.get("src"):
            self.urls.append(attributes["src"])
        if tag == "link" and attributes.get("href"):
            self.urls.append(attributes["href"])


index = (SITE / "index.html").read_text(encoding="utf-8")
controller = (SITE / "js" / "main.js").read_text(encoding="utf-8")
parser = Assets()
parser.feed(index)
for url in parser.urls:
    parts = urlsplit(url)
    if parts.scheme or parts.netloc:
        raise AssertionError(f"external required asset: {url}")
    assert (SITE / parts.path).is_file(), f"missing local asset: {url}"

assert "epdiy.cn" not in index + controller
assert "tsl0922.github.io" not in index + controller
workflow = (ROOT / ".github" / "workflows" / "static.yml").read_text(encoding="utf-8")
assert "path: 'html'" in workflow
assert 'branches: ["main"]' in workflow
print("GitHub Pages static assets/workflow test: PASS")
