import json
import subprocess
import time


def readUrls(filename):
    with open(filename, "r") as f:
        urls = f.read().split()
    return urls


def fetchUrl(urlString):
    out = subprocess.run(
        ["curl", urlString],
        text=True,
        check=True,
        capture_output=True,
    )
    return out.stdout


def parseHTMLTag(htmlString, tag, closeTag):
    matches = []
    tagLen = len(tag)

    insideLi = False
    liStartOffset = -1
    liEndOffset = -1
    i = tagLen
    while i < len(htmlString):
        if not insideLi:
            if tag == htmlString[i - (tagLen - 1) : i + 1]:
                insideLi = True
                liStartOffset = i + 1
                liEndOffset = liStartOffset
        else:
            liEndOffset += 1
            if htmlString[i - (tagLen) : i + 1] == closeTag:
                matches.append(htmlString[liStartOffset : liEndOffset - (tagLen + 1)])
                insideLi = False
                continue
        i += 1

    return matches


def parseTimestamp(time_str):
    s = time_str.strip()  # NOTE: need tmp var for new length check below
    clean = s[1 : len(s) - 1]
    slots = clean.split(":")
    if len(slots) == 3:
        sec = (int(slots[0]) * 60 * 60) + (int(slots[1]) * 60) + int(slots[2])
    elif len(slots) == 2:
        sec = (int(slots[0]) * 60) + int(slots[1])
    else:
        assert False
        sec = -1

    return clean, sec


def main():
    qna_urls = readUrls("./q_and_a.csv")
    all_questions = []
    for url in qna_urls:
        print(f"Fetching ... {url} ...")

        html = fetchUrl("https://www.computerenhance.com/p/q-and-a-33-2023-11-06")
        questions_raw = parseHTMLTag(html, "<li>", "</li>")
        for r in questions_raw:
            print(f"{r}\n\n")

        # questions = []
        # for q in questions_raw:
        #     timestamp = parseHTMLTag(q, "<strong>", "</strong>")
        #     question = parseHTMLTag(q, "<span>", "</span>")
        #     if timestamp and question:
        #         str_time, sec = parseTimestamp(timestamp[0])
        #         questions.append(
        #             {
        #                 "timestamp": {"str": str_time, "sec": sec},
        #                 "question": question[0].strip(),
        #             }
        #         )
        #
        # all_questions.append({"url": url, "questions": questions})
        time.sleep(1)
        break

    # with open("./q_and_a_extracted.json", "w") as f:
    #     json.dump(all_questions, f)


main()
