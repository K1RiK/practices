import json

base = ["ip1 time1 url1", "ip2 time2 url2", "ip1 time3 url3", "ip1 time3 url3", "ip1 time5 url1"]
base = [el.split(' ') for el in base]
base = [{
    "Id": i,
    "Pid": None,
    "URL": el[2],
    "SourceIP": el[0],
    "TimeInterval": el[1],
    "Count": None
    } for i, el in enumerate(base)]
print("BASE:", json.dumps(base, indent=2))
dimensinos = ["SourceIP", "URL", "TimeInterval"]
d1, d2, d3 = dimensinos
data = dict()
for d in base:
    p1, p2, p3 = d[d1], d[d2], d[d3]
    if p1 not in data.keys():
        data[p1] = {p2: [p3]}
        continue
    if p2 not in data[p1].keys():
        data[p1][p2] = [p3]
    else:
        data[p1][p2].append(p3)
print("JSON:", json.dumps(data, indent=2))
# Pids = [0]
# for b in base:
#     p1, p2, p3 = b[d1], b[d2], b[d3]
#     if b["Id"] in Pids:
#         p2 = None
#         p3 = None
#     else:
#         print(Pids)
#         ad = 1
#         for pid in Pids:
#             if p1 == base[pid][d1]:
#                 b["Pid"] = pid
#                 ad = 0
#                 break
#         if ad:
#             Pids.append(b["Id"])
#             p2 = None
#             p3 = None
# print("REPORT:", json.dumps(base, indent=2))
