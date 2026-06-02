from playwright.sync_api import sync_playwright

url = "https://example.com"

with sync_playwright() as p:
    browser = p.chromium.launch(headless=True)
    page = browser.new_page()
    page.goto(url, wait_until="networkidle")
    print(page.content())
    browser.close()
