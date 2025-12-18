# coding=utf-8

"""
Selection
"""

from collections import OrderedDict
from datetime import datetime
from pathlib import Path
from logic.helper import PrettyText

class Selection:
    def __init__(self, model):

        self.__model = model

        # Bases
        self.__odic_valid_base = OrderedDict()

        # Not valid
        self.__odic_not_valid_base = OrderedDict()
        self.__lst_not_valid_part_station = []
        self.__lst_not_valid_part_year = []
        self.__lst_not_valid_part_month = []

        # Iterator
        self.__s_iterator_mode_valid = "all" # all/valid/not_valid
        self.__s_iterator_mode_type = "all/part_result/file_with_path/file_no_path" # all/part_result/file_with_path/file_no_path
        self.__lst_iterator_items = None # Iterator items
        self.__set_used_paths = None # Iterator paths
        self.__iterator = None

    def __str__(self):
        return self.info()

    # --- Iterator

    def __iter__(self):
        if self.is_ready():
            self.__iterator = iter(self.__lst_iterator_items)
            return self
        else:
            raise Exception("Iterator not initialized")

    def __next__(self):
        while True:
            s_base, s_file, s_path = next(self.__iterator)  # StopIteration kommt von alleine
            return s_base, s_file, s_path

    def __len__(self):
        return self.get_length()

    """
    Setup Iterator
    Args:
        valid (str): all,valid,not_valid
        type (str): all,file_with_path,file_no_path,part_result"
    """
    def init(self, valid="all", type="all"):
        self.__set_iteration_mode_type(type)
        self.__set_iteration_mode_valid(valid)
        # Init iterator data
        self.__lst_iterator_items = []
        self.__set_used_paths = set()
        if self.get_iteration_mode_valid() in ["valid", "all"]:  # Eval: mode-valid: valid/not valid/all
            for s_base, set_file_path in self.get_base_valid(
                    self.get_iteration_mode_type()).items():  # Eval: mode-type: filw_with_path,file_no_path/part_result/all
                for s_file, s_path in set_file_path:
                    self.__lst_iterator_items.append((s_base, s_file, s_path))
                    self.__set_used_paths.add(s_path)
        if self.get_iteration_mode_valid() in ["not_valid", "all"]:
            for s_base, set_file_path in self.get_base_not_valid(self.get_iteration_mode_type()).items():
                for s_file, s_path in set_file_path:
                    self.__set_used_paths.add(s_path)
                    self.__lst_iterator_items.append((s_base, s_file, s_path))

    def __set_iteration_mode_valid(self, s_txt):
        if s_txt not in ["all", "valid", "not_valid"]:
            s_txt = "all"
        self.__s_iterator_mode_valid = s_txt

    def __set_iteration_mode_type(self, s_txt):
        if s_txt not in ["all", "file_with_path", "file_no_path", "part_result"]:
            s_txt = "all"
        self.__s_iterator_mode_type = s_txt

    def get_iteration_mode_valid(self):
        return self.__s_iterator_mode_valid

    def get_iteration_mode_type(self):
        return self.__s_iterator_mode_type

    def is_ready(self):
        return self.__lst_iterator_items is not None

    def get_length(self):
        return len(self.__lst_iterator_items) if self.is_ready() else 0

    def is_empty(self):
        return True if self.get_length() == 0 else False

    def get_used_paths(self):
        return self.__set_used_paths

    # --- Info

    def info_valid(self):
        s_txt = ""
        if self.has_valid("part_result"):
            odic = self.get_base_valid("part_result")
            lst_tmp = [f"{k}" for k, set_file_path in odic.items()]
            s_txt += f'> from part ({self.get_base_num_valid("part_result")}x)\n'
            s_txt += f'{PrettyText.lst2strC(lst_tmp, 5, i_block_items_max=20)}\n'
        if self.has_valid("file_no_path"):
            odic = self.get_base_valid("file_no_path")
            lst_tmp = []
            for k, set_file_path in odic.items():
                for s_file, s_path in set_file_path:
                    lst_tmp.append(f"{k}[{s_file}]")
            s_txt += f'> from file (no path) ({self.get_base_num_valid("file_no_path")}x)\n'
            s_txt += f'{PrettyText.lst2strC(lst_tmp, 5, i_block_items_max=20)}\n'
        if self.has_valid("file_with_path"):
            odic = self.get_base_valid("file_with_path")
            lst_tmp = []
            for k, set_file_path in odic.items():
                for s_file, s_path in set_file_path:
                    lst_tmp.append(f"{k}[{s_file},{s_path}]")
            s_txt += f'> from file (with path) ({self.get_base_num_valid("file_with_path")}x)\n'
            s_txt += f'{PrettyText.lst2strC(lst_tmp, 5, i_block_items_max=20)}\n\n'
        return s_txt

    def info_not_valid(self):
        s_txt = ""
        if self.has_part_not_valid():
            s_txt += f'> not valid part ({self.get_part_not_valid_num()}x)\n'
            if self.has_part_not_valid("station"):
                s_txt += f'station ({self.get_part_not_valid_num("station")}x): {PrettyText.lst2strC(self.get_part_not_valid("station"))}\n'
            if self.has_part_not_valid("month"):
                s_txt += f'month ({self.get_part_not_valid_num("month")}x): {PrettyText.lst2strC(self.get_part_not_valid("month"))}\n'
            if self.has_part_not_valid("year"):
                s_txt += f'year ({self.get_part_not_valid_num("year")}x): {PrettyText.lst2strC(self.get_part_not_valid("year"))}\n'
        if self.has_not_valid("part_result"):
            odic = self.get_base_not_valid("part_result")
            lst_tmp = [f"{k}" for k, set_file_path in odic.items()]
            s_txt += f'> from not valid part ({self.get_base_num_not_valid("part_result")}x)\n'
            s_txt += f'{PrettyText.lst2strC(lst_tmp, 5, i_block_items_max=20)}\n'
        if self.has_not_valid("file_no_path"):
            odic = self.get_base_not_valid("file_no_path")
            lst_tmp = []
            for k, set_file_path in odic.items():
                for s_file, s_path in set_file_path:
                    lst_tmp.append(f"{k}[{s_file}]")
            s_txt += f'> from file (no path) ({self.get_base_num_not_valid("file_no_path")}x)\n'
            s_txt += f'{PrettyText.lst2strC(lst_tmp, 5, i_block_items_max=20)}\n'
        if self.has_not_valid("file_with_path"):
            odic = self.get_base_not_valid("file_with_path")
            lst_tmp = []
            for k, set_file_path in odic.items():
                for s_file, s_path in set_file_path:
                    lst_tmp.append(f"{k}[{s_file},{s_path}]")
            s_txt += f'> from file (with path) ({self.get_base_num_not_valid("file_with_path")}x)\n'
            s_txt += f'{PrettyText.lst2strC(lst_tmp, 5, i_block_items_max=20)}\n'
        return s_txt

    def info(self):
        s_txt = "* Selector\n\n"
        s_txt += f"--- Iterator\n\n"
        s_txt += f"Iteration mode valid: {self.get_iteration_mode_valid()}\n"
        s_txt += f"Iteration mode type: {self.get_iteration_mode_type()}\n"
        s_txt += f'Iteration initialized: {"yes" if self.is_ready() else "no"}\n'
        if self.is_ready():
            s_txt += f'Iterator len: {self.get_length()}\n'
        s_txt += '\n'
        if self.is_empty():
            # Empty
            s_txt += f"<empty>\n"
        else:
            # Not empty
            if self.has_valid():
                s_txt += f'--- Valid ({self.get_base_num_valid()}x)\n\n'
                s_txt += self.info_valid()
                s_txt += "\n"
            if self.has_not_valid():
                s_txt += f'--- Not valid ({self.get_base_num_not_valid()}x)\n\n'
                s_txt += self.info_not_valid()
                s_txt += "\n"
        s_txt = PrettyText.clean_wizard(s_txt, max_one_empty_row=True)
        return s_txt

    # --- internal helper

    """
    Wrapper to add data to internal data structures 
    Args:
        s_type (str): base,not_valid_base,not_valid_station,not_valid_year,not_valid_month
        s_key (str): base name
        val: various
    """
    def __add_to_internal_data(self, s_type, s_key, val=None):
        if s_key is None:
            return

        # --- odics
        # Base
        if s_type in ["base", "not_valid_base"]:
            s_file = val
            # if possible eval file -> name & path
            s_file_name = None
            s_file_path = None
            try:
                s_file = Path(s_file)
                s_file_name = v if (v:= str(s_file.name).lower()) not in ["", None] else None
                s_file_path = v if (v:= str(s_file.parent).lower()) not in ["", None, "."] else None
            except:
                pass
            if s_type == "base":
                if s_key not in self.__odic_valid_base:
                    self.__odic_valid_base[s_key] = set()
                self.__odic_valid_base[s_key].add((s_file_name, s_file_path))
            elif s_type == "not_valid_base":
                if s_key not in self.__odic_not_valid_base:
                    self.__odic_not_valid_base[s_key] = set()
                self.__odic_not_valid_base[s_key].add((s_file_name, s_file_path))
            return

        # --- lists
        lst = None
        # Not valid
        if s_type == "not_valid_station":
            lst = self.__lst_not_valid_part_station
        elif s_type == "not_valid_month":
            lst = self.__lst_not_valid_part_month
        elif s_type == "not_valid_year":
            lst = self.__lst_not_valid_part_year
        if lst != None and s_key not in lst:
            lst.append(s_key)

    # --- Parts

    """
    Get not valid parts
    Args
        s_type (str): station,year,month
    """
    def get_part_not_valid(self, s_type):
        if s_type == "station":
            return self.__lst_not_valid_part_station
        elif s_type == "year":
            return self.__lst_not_valid_part_year
        elif s_type == "month":
            return self.__lst_not_valid_part_month
        else:
            return None

    """
    Get number of not valid parts
    Args
        s_type (str): station,year,month
    """
    def get_part_not_valid_num(self, s_type=None):
        if s_type == "station":
            return len(self.__lst_not_valid_part_station)
        elif s_type == "year":
            return len(self.__lst_not_valid_part_year)
        elif s_type == "month":
            return len(self.__lst_not_valid_part_month)
        elif s_type is None:
            return self.get_part_not_valid_num("station") + self.get_part_not_valid_num("year") + self.get_part_not_valid_num("month")
        else:
            return None

    # Has not valid parts
    def has_part_not_valid(self, s_type=None):
        return self.get_part_not_valid_num(s_type) > 0

    # --- Base names

    """
    Get valid base names
    Args:
        s_type (str): all,file_with_path,file_no_path,file_all,part_result
    """
    def get_base_valid(self, s_type=None):
        return self.__get_base(s_type, True)

    # Get not valid base names
    def get_base_not_valid(self, s_type=None):
        return self.__get_base(s_type, False)

    # Helper
    def __get_base(self, s_type, b_valid):
        if b_valid:
            odic_data = self.__odic_valid_base
        else:
            odic_data = self.__odic_not_valid_base
        odic_out = None
        if s_type in ["file_with_path"]:
            odic_out = OrderedDict()
            for k, set_file_path in odic_data.items():
                for s_file, s_path in list(set_file_path):
                    if s_file is not None and s_path is not None:
                        if k not in odic_out:
                            odic_out[k] = set()
                        odic_out[k].add((s_file, s_path))
            return odic_out
        elif s_type in ["file_no_path"]:
            odic_out = OrderedDict()
            for k, set_file_path in odic_data.items():
                for s_file, s_path in list(set_file_path):
                    if s_file is not None and s_path is None:
                        if k not in odic_out:
                            odic_out[k] = set()
                        odic_out[k].add((s_file, s_path))
            return odic_out
        elif s_type == "file_all":
            odic_out = OrderedDict()
            for k, set_file_path in odic_data.items():
                for s_file, s_path in list(set_file_path):
                    if s_file is not None:
                        if k not in odic_out:
                            odic_out[k] = set()
                        odic_out[k].add((s_file, s_path))
            return odic_out
        elif s_type == "part_result":
            odic_out = OrderedDict()
            for k, set_file_path in odic_data.items():
                for s_file, s_path in list(set_file_path):
                    if s_file is None and s_path is None:
                        if k not in odic_out:
                            odic_out[k] = set()
                        odic_out[k].add((s_file, s_path))
            return odic_out
        elif s_type in ["all", None]:
            odic_out = odic_data.copy()
        return odic_out

    # Get number of valid base names
    def get_base_num_valid(self, s_type=None):
        return self.__get_base_num(s_type, True)

    # Get number of not valid base names
    def get_base_num_not_valid(self, s_type=None):
        return self.__get_base_num(s_type, False)

    # Helper
    def __get_base_num(self, s_type, b_valid):
        try:
            odic = self.__get_base(s_type, b_valid)
            i_len = 0
            for s_base, set_file_path in odic.items():
                i_len += len(set_file_path)
            return i_len
        except:
            return None

    # Has valids?
    def has_valid(self, s_type=None):
        return self.__has_valid_not_valid(s_type, True)

    # Has not valids?
    def has_not_valid(self, s_type=None):
        return self.__has_valid_not_valid(s_type, False)

    # Helper
    def __has_valid_not_valid(self, s_type, b_valid):
        try:
            return self.__get_base_num(s_type, b_valid) > 0
        except:
            return None

    # --- Reset

    def reset(self):
        self.__odic_valid_base = OrderedDict()
        self.__lst_not_valid_part_month = []
        self.__lst_not_valid_part_year = []
        self.__lst_not_valid_part_station = []
        self.__iterator = []

    # --- load

    # Load filenames
    def load_filenames(self, lst_files):
        for s_file in lst_files:
            s_file = Path(s_file.lower())
            s_file_name = s_file.name
            s_file_path = s_file.parent
            b_valid, s_station, i_month, i_year_short = self.tool_check_filename(s_file_name)
            s_base = self.tool_create_base_from_file(s_file_name)
            if b_valid:
                self.__add_to_internal_data("base", s_base, s_file)
            else:
                self.__add_to_internal_data("not_valid_base", s_base, s_file)
        # Sort
        self.__odic_valid_base = self.tool_sort_base(self.__odic_valid_base)

    # Load parts
    def load_parts(self, lst_station, lst_month, lst_year):
        # empty -> all
        if len(lst_station) == 0:
            lst_station = self.__model.get_stations_lower()
        if len(lst_month) == 0:
            lst_month = self.__model.get_months_int()
        if len(lst_year) == 0:
            lst_year = self.__model.get_years()

        # A little cleaning
        lst_station = [s_station.lower() for s_station in lst_station] # lower
        # Eval
        for s_station in lst_station:
            s_station = s_station.lower()
            for i_year in lst_year:
                for i_month in lst_month:
                    b_valid, s_station_not_valid, i_month_not_valid, i_year_not_valid,  = self.tool_check_parts(s_station, i_month, i_year)
                    s_base = self.tool_create_base_from_parts(s_station, i_month, i_year)
                    if b_valid:
                        self.__add_to_internal_data("base", s_base)
                    else:
                        self.__add_to_internal_data("not_valid_station", s_station_not_valid)
                        self.__add_to_internal_data("not_valid_year", i_year_not_valid)
                        self.__add_to_internal_data("not_valid_month", i_month_not_valid)
                        self.__add_to_internal_data("not_valid_base", s_base)
        # Sort
        self.__odic_valid_base = self.tool_sort_base(self.__odic_valid_base)

    # --- Tools

    # Sort data-structure (OrderedDictionary od List) with base names (more difficult cause of weird sequence: name_month_year)
    @staticmethod
    def tool_sort_base(data):

        # Helper Sort
        def tp_station_date(s_entry):
            s_entry = Selection.tool_create_base_from_file(s_entry) # transform all to base
            abc = s_entry[:3]  # Station 123
            mm = s_entry[3:5]  # Month 45
            yy = s_entry[5:]  # Year 67
            # Convert two-digit year + month to date
            # %y interprets two-digit year -> 00–68 = 2000–2068, 69–99 = 1969–1999
            date = datetime.strptime(f"{yy}{mm}", "%y%m")
            return (abc, date)

        # ---

        if isinstance(data, OrderedDict) or isinstance(data, dict):
            # OrderedDict or Dictionary
            sorted_items = sorted(data.items(), key=lambda item: tp_station_date(item[0]))
            return OrderedDict(sorted_items)
        elif isinstance(data, list):
            # List
            sorted_items = sorted(data, key=lambda item: tp_station_date(item))
            return sorted_items
        else:
            return None

    # Sort list with files (more difficult cause of weird sequence: name_month_year)
    @staticmethod
    def tool_sort_files(lst_files):
        dic_tmp = {}
        for s_file in lst_files:
            s_base = Path(s_file).name.removesuffix(".dat.gz")
            dic_tmp[str(s_base)] = None
        return list(Selection.tool_sort_base(dic_tmp).keys())

    # Convert short year to long year version
    def tool_year_short_to_long(self, i_year_short):
        # check: short (2 digits)
        if len(str(i_year_short)) > 2:
            return None
        # get lowest year in short version
        i_year_lowest_short = self.__model.get_year_lowest_short()
        # catch outlyers
        if i_year_short > 99:
            i_year_short = 99
        if i_year_short < 0:
            i_year_short = 0

        if (i_year_short >= i_year_lowest_short) and (i_year_short <= 99):
            i_year_long = int(f"19{i_year_short:02}")
        else:
            i_year_long = int(f"20{i_year_short:02}")
        return i_year_long

    # Convert long version to short version
    def tool_year_long_to_short(self, i_year_long):
        # check: long (4 digits)
        if len(str(i_year_long)) != 4:
            return None
        # Check
        if len(i_year_long) == 2:
            return i_year_long
        if len(i_year_long) == 4:
            return int(f"{i_year_long}"[2:4])
        else:
            return None

    # Create base name from parts
    def tool_create_base_from_parts(self, s_station, i_month, i_year):
        s_year = f"{i_year}"[-2:]
        s_month = f"{i_month:02d}"
        s_base = f"{s_station.lower()}{s_month}{s_year}"
        return s_base

    # Create base name from file
    @staticmethod
    def tool_create_base_from_file(s_file):
        path_file = Path(s_file)
        s_file = str(path_file.name)
        return s_file[:7]

    # Check if parts are valid?
    def tool_check_parts(self, s_station, i_month, i_year_long):
        b_valid = True
        s_station_invalid = None
        i_month_invalid = None
        i_year_long_invalid = None
        if i_month not in self.__model.get_months_int():
            b_valid = False
            i_month_invalid = i_month
        if i_year_long not in self.__model.get_years():
            b_valid = False
            i_year_long_invalid = i_year_long
        if s_station not in self.__model.get_stations_lower():
            b_valid = False
            s_station_invalid = s_station
        return b_valid, s_station_invalid, i_month_invalid, i_year_long_invalid

    # Check filename is valid?
    def tool_check_filename(self, s_file):
        b_valid = False
        s_station = None
        i_month = None
        i_year = None
        i_year_long = None
        if s_file.endswith(".dat.gz") or s_file.endswith(".dat") or s_file.endswith(".rep.txt.gz") or s_file.endswith(".rep.txt"):
            try:
                s_station = s_file[0:3]
                i_month = int(s_file[3:5])
                i_year_short = int(s_file[5:7])
                i_year_long = self.tool_year_short_to_long(i_year_short)
                b_valid, s_station_not_valid, i_month_not_valid, i_year_long_not_valid  = self.tool_check_parts(s_station, i_month, i_year_long)
            except:
                pass
        else:
            pass
        return b_valid, s_station, i_month, i_year_long
