#!/usr/local/bin/python3

# coding=utf-8

"""
Test
"""
import os
from pathlib import Path

"""
TODO
"""

from logic.selection import Selection
from mvc.model import Model

#quit()

# Download new
m = Model()
m.set_debug(False)
m.set_verbose(True)

tm = m.get_task_manager()
ldb = m.get_local_working_database()
ldb = m.get_local_working_database()
lup = m.get_bsrn_id_system()

#quit()

b_unzip = False
b_report = True
b_export = True
b_check_availability = False
b_buffer = True
b_buffer_refresh = False
s_working_dir = m.get_working_dir()

ldb.db_buffer_set_refresh(b_buffer_refresh)
ldb.db_buffer_set_active(b_buffer)

# bsrnid = m.get_bsrn_id_system()
# val = bsrnid.get_data("methods_wrmc", "86009")
#ldb.db_export_to_zip()

lst_month = []
lst_station = []
lst_year = []

#lst_station = ["ABS", "ALE", "BOS", "GAY", "DON", "CYL", "EFS", "NPT", "RLM"]
#lst_station = []
lst_station = ["ABS", "IZA"]
lst_year = [2023]
lst_month = [1]

# --- Process Data in working dir
s_working_dir1 = Path(s_working_dir, "test")
s_working_dir2 = Path(s_working_dir, "test2")
sel = Selection(m)

lst_files1 = [str(f) for f in s_working_dir1.iterdir() if f.is_file() and f.name.endswith(".dat.gz")]
#lst_files2 = [str(f) for f in s_working_dir2.iterdir() if f.is_file() and f.name.endswith(".dat.gz")]
sel.load_filenames(lst_files1)
#sel.load_filenames(lst_files2)
#sel.load_parts(lst_station, lst_month, lst_year)
sel.init()


res = tm.process_station_data(sel, report=b_report, export_data=b_export, export_data_recs=["0100", "0300"])
s_log = res.get_data("log")
print(s_log)
quit()

res = tm.download_station_data_and_check(sel, s_working_dir, report=b_report)
s_log = res.get_data("log")
print(s_log)
quit()

#print(sel)
#res = tm.process_station_data(sel, report=b_report, export_data=b_export, export_data_recs=["0001", "0002", "0003", "0004", "0005", "0006", "0007", "0008", "0009"])

print("---")
print(s_log)
quit()

# --- Dwonlaod, Import and Report data
sel = Selection(m)
lst_files = ['abs1223.dat.gz']
sel.load_filenames(lst_files)
s_working_dir = Path(s_working_dir, "now")

res = tm.download_station_data_and_check(sel, s_working_dir, report=True)
s_log = res.get_data("log")
# print(s_log)
quit()

ldb.db_buffer_save_to_disk()

#lst_files = ["bar0123.dat", "bar0223.dat.gz", "pay0123.dat.gz","gay0120.dat.gz"]
#lst_files = ["bar0123.dat", "bar0123.dat", "bar0223", "pay0123.dat.gz","gay0120.dat.gz", "/Users/pkloss/_temp/bsrn_work/now/abs1223.dat.gz", "/Users/pkloss/_temp/bsrn_work/now/abs0123.dat.gz", "/Users/pkloss/_temp/bsrn_work/now/abs0190.dat.gz"]
#lst_files = ['/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs1223.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0523.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0423.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0823.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0923.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0223.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0323.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0623.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0723.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs1123.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs1023.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0123.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs1223.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0523.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0423.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0823.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0923.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0223.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0323.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0623.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0723.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs1123.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs1023.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_15-10-55/abs0123.dat.gz']
#lst_files = ['/pfad1/abs1223.dat.gz', '/pfad2/abs1223.dat.gz', 'abs1223.dat.gz']
# lst_files_bad = [ 'abs1230.dat.gz', "/pfad1/abs1230.dat.gz", "/pfad2/abs1230.dat.gz", "/xyz/abs0125.dat.gz"]
# lst_files = ['/Users/pkloss/_temp/bsrn_work/test/pay0123.dat', '/Users/pkloss/_temp/bsrn_work/test/abs1223.dat.gz', '/Users/pkloss/_temp/bsrn_work/test/abs0123.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs1223.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0523.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0423.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0823.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0923.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0223.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0323.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0623.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0723.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs1123.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs1023.dat.gz', '/Users/pkloss/_temp/bsrn_work/bak/2025-09-12_14-51-44/abs0123.dat.gz']

# lst_files = [
#     "/Users/pkloss/_temp/bsrn_work/test/abs1223.dat.gz",
#     "/Users/pkloss/_temp/bsrn_work/test/abs0123.dat.gz",
#     "/Users/pkloss/_temp/bsrn_work/test/pay0120.dat.gz",
#     "/Users/pkloss/_temp/bsrn_work/test/pay0123.dat.gz"]
#
# s_working_dir = Path(s_working_dir, "test_big")
# sel = Selection(m)
# lst_files = [str(f) for f in s_working_dir.iterdir() if f.is_file()]
# sel.load_filenames(lst_files)

# #sel.set_iteration_mode("files")
# sel.load_filenames(lst_files)
# #sel.load_filenames(lst_files_bad)
# #sel.init(valid="valid", type="file_with_path")

# res = tm.process_station_data(sel, report=True)
# s_log = res.get_data("log")
# print("---")
# print(s_log)
# quit()
# print(sel)
# print(len(sel))
# for k, s_file, s_path in sel:
#     print(k, s_file, s_path)
# quit()
#tmp = sel.get_base_valid("all")
#print(tmp)
#quit()
#sel.load_parts(lst_station, lst_month, lst_year)

#print(sel.get_base_num_not_valid("all"))
# print(sel)
#print(sel.info_not_valid())
# quit()
# for i_idx, (s_base, s_file, s_path) in enumerate(sel):
#     print(i_idx, len(sel), s_base, s_file, s_path)

# # Downlaod
#res = tm.download_station_data_and_check(sel, s_working_dir, unzip=b_unzip, report=b_report)

# Availability
res = tm.check_availability_on_server(lst_station, str(Path(s_working_dir, "now")))
s_log = res.get_data("log")
print()
#print(s_log)

quit()









