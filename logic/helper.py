# coding=utf-8

"""
Helper
"""

import datetime, os, random, re, string, sys, time, traceback
from collections import OrderedDict

# Result
class Result:
    def __init__(self, **kwargs):

        """
        Kwargs:
             name (str): Name of the action that triggered the result
             err (list/str): Error messages
             warn (list/str): Warnings
             info (list/str): Information
             data (dict): Data
        """

        self.__s_name = None
        self.__lstErr = []
        self.__lstWarn = []
        self.__lstInfo = []
        self.__dicData = {}

        # Name
        if "name" in kwargs:
            self.__s_name = kwargs['name']
        # Sowohl Strings als auch Listen werden den Error/Warnungs/Informations-Listen hinzugefuegt
        if "err" in kwargs:
            self.add_err(kwargs['err'])
        if "warn" in kwargs:
            self.add_warn(kwargs['warn'])
        if "info" in kwargs:
            self.add_info(kwargs['info'])

        # Daten müssen in einem Dictionary liegen um übergeben werden zu können
        if "data" in kwargs:
            self.add_data_dict(kwargs['data'])

    def __str__(self):
        if not self.is_anything():
            return "Nothing special occurred ..."
        return ("*** Name\n" + self.get_name() + "\n" if self.get_name() is not None else "")+self.get_err_warn_info_string()

    def get_err_warn_info_string(self):
        s_info =""
        if self.is_err():
            s_info += "*** Errors (" + str(self.get_err_no()) + ")\n" + self.get_err() + "\n"
        if self.is_warn():
            s_info += "*** Warnings (" + str(self.get_warn_no()) + ")\n" + self.get_warn() + "\n"
        if self.is_info():
            s_info += "*** Info (" + str(self.get_info_no()) + ")\n" + self.get_info() + "\n"
        return s_info[:-1]

    def add_result(self, res):
        """
        Adds the contents of another result class to a result class

        Args:
            res (tools.coding.Result): Result

        Returns:
            res (tools.coding.Result): Result
        """

        self.add_err(res.get_err_lst())
        self.add_warn(res.get_warn_lst())
        self.add_info(res.get_info_lst())
        self.add_data_dict(res.get_data())

    def add_err(self, dat):
        if type(dat) is list:
            self.__lstErr += dat
        else:
            self.__lstErr.append(dat)

    def add_warn(self, dat):
        if type(dat) is list:
            self.__lstWarn += dat
        else:
            self.__lstWarn.append(dat)

    def add_info(self, dat):
        if type(dat) is list:
            self.__lstInfo += dat
        else:
            self.__lstInfo.append(dat)

    def add_data(self, key, val):
        if key in self.__dicData:
            print("Internal message [add data to result object]: key exists -> old data is overridden: key=" + str(key))
        self.__dicData[key] = val

    def add_data_dict(self, dat):
        if type(dat) is dict:
            self.__dicData.update(dat)
        elif dat is None:
            pass
        else:
            print("Internal message [add data to result object]: Result data is not a dictionary and therefore not added to Result-Object: " + str(dat) + "(type="+str(type(dat))+")")

    def is_ok(self):
        return not self.is_err()

    def is_err(self):
        return self.__lstErr != []

    def is_warn(self):
        return self.__lstWarn != []

    def is_info(self):
        return self.__lstInfo != []

    def is_data(self):
        return self.__dicData != dict()

    def is_anything(self):
        if self.is_err() or self.is_warn() or self.is_info():
            return True
        else:
            return False

    def set_name(self, sName):
        self.__s_name = sName

    def get_name(self):
        return self.__s_name

    def get_err_lst(self):
        return self.__lstErr

    def get_warn_lst(self):
        return self.__lstWarn

    def get_info_lst(self):
        return self.__lstInfo

    def get_err_no(self):
        return len(self.__lstErr)

    def get_warn_no(self):
        return len(self.__lstWarn)

    def get_info_no(self):
        return len(self.__lstInfo)

    def get_err(self):
        return "\n".join(self.__lstErr)

    def get_warn(self):
        return "\n".join(self.__lstWarn)

    def get_info(self):
        return "\n".join(self.__lstInfo)

    def get_data(self, key=None):
        if key is None:
            return self.__dicData
        elif key in self.__dicData:
            return self.__dicData[key]
        else:
            return None

# Smartprinter
class SmartPrinter:

    def __init__(self, **kwargs):
        """
        Kwargs:
            verbose (bool): Verbose
            debug (bool): Debug
            model (Model): Model
        """

        # Instancew variables
        # Default
        self.__b_verbose = False
        self.__b_debug = False
        self.__model = None

        self.__b_last_cmd_progress_cli = False # Tweak to notice when a port bar is running in CLI mode and a different output (-> Return) appears in the middle of it

        self.__input_answer = None

        # Distribute values from constructor parameter list
        if "verbose" in kwargs:
            self.__b_verbose = kwargs['verbose']
        if "debug" in kwargs:
            self.__b_debug = kwargs['debug']
        if "model" in kwargs:
            self.__model = kwargs['model']

    def input_ask_combo(self, s_title, s_label, lst_choices):

        if self.is_gui():
            # Gui
            # Give Gui the command to open a combo box
            self.__model.get_observer().input_combo(s_title, s_label, lst_choices)
            return
        else:
            # Cli
            self.__print_console(s_title)
            self.__print_console("Choose from: ")
            iFunde = 0
            for i, sText in enumerate(lst_choices):
                self.__print_console(str(i) + ": " + sText)
                iFunde += 1
            b_eingabe_ok = False
            while True:
                ein = input("Choose (0-"+str(iFunde-1)+"): ")
                try:
                    if int(ein) >= 0 and int(ein) < iFunde:
                        self.set_input_answer(int(ein))
                        return
                except:
                    pass

    def set_input_answer(self, data):
        self.__input_answer = data

    def get_input_answer(self):
        data = self.__input_answer
        if self.__input_answer is not None: # Delete again if something has arrived
            self.__input_answer = None
        return data

    def is_gui(self):
        return self.__model is not None and self.__model.get_observer() is not None

    def set_verbosity(self, verbose, debug):
        self.__b_verbose = verbose
        self.__b_debug = debug

    def is_verbose(self):
        return True if self.__b_verbose or self.__model != None and self.__model.is_verbose() else False

    def is_debug(self):
        return True if self.__b_debug or self.__model != None and self.__model.is_debug() else False

    def normal(self, text="", appendix="\n"):
        if (self.is_gui()):
            self.__print_gui(text, appendix)  # GUI
        else:
            self.__print_console(text, appendix)  # CLI

    def verbose(self, s_text="", appendix="\n"):
        if (self.is_gui()) and (self.__model.is_verbose() == True or self.__model.is_debug() == True):
            self.__print_gui(s_text, appendix)  # GUI
        elif self.__model != None and (self.__model.is_verbose() == True or self.__model.is_debug() == True):
            self.__print_console(s_text, appendix)  # CLI
        elif self.__b_verbose == True or self.__b_debug == True:
            self.__print_console(s_text, appendix)  # CLI

    def debug(self, text="", appendix="\n"):
        if (self.is_gui()) and self.__model.is_debug() == True:
            self.__print_gui(text, appendix)  # GUI
        elif self.__model != None and self.__model.is_debug() == True:
            self.__print_console(text, appendix)  # CLI
        elif self.__b_debug == True:
            self.__print_console(text, appendix)  # CLI

    def __print_console(self, text="", appendix="\n"):
        if self.__b_last_cmd_progress_cli:
            self.__b_last_cmd_progress_cli = False
            ERASE_LINE = '\x1b[2K'
            CURSOR_BACK = '\r'
            sys.stdout.write(CURSOR_BACK)
            sys.stdout.write(ERASE_LINE)
            sys.stdout.flush()
        print(text, end=appendix)

    def __print_gui(self, s_text="", appendix="\n"):
        if self.is_gui():
            self.__model.get_observer().print_gui(s_text, appendix)

    def get_pretty_table(self, lst_list, b_header=True):
        sTable = ""
        MaxRow = [max(map(len, col)) for col in zip(*lst_list)]
        for nr, row in enumerate(lst_list):
            # Content
            sTable += " | ".join((val.ljust(Max) for val, Max in zip(row, MaxRow))) + "\n"
            # Header separator
            if b_header and nr == 0:
                sTable += "-|-".join((("-" * Max).ljust(Max) for val, Max in zip(row, MaxRow))) + "\n"
        return sTable[:-1]

    def status(self, s_text =""):
        if self.__model is not None and self.__model.get_observer() is not None:
            self.__model.get_observer().set_status(s_text)

    def buffer(self):
        if self.__model is not None and self.__model.get_observer() is not None:
            self.__model.get_observer().update_buffer()

    def workflow(self, s_text="", i_step=0, i_step_max=0):
        if self.is_gui():
            self.__model.get_observer().set_workflow(s_text, i_step, i_step_max)

    def progress(self, i_val_now="pulse", i_val_max=100, i_steps=100, **kwargs):
        """
        Progressbar

        Args:
            i_val_now (int): actual val
            i_val_max (int): max val
            i_steps (int): steps

        Kwargs:
            text (str): Text to be added to the progress bar in GUI mode
            abs (bool): Should the absolute steps be displayed in addition to the percentage?
            cli (bool): Should the progress also be displayed in CLI mode?
            problem (str): problem
            abort (bool): Abort button
            status (str): status CLI
            info (str): info
        """

        s_text = ""
        s_status = ""
        b_percent = False
        b_abs = False
        b_cli = False
        b_abort = True

        i_max_out = 0 # Outliers upwards
        i_min_out = 0  # Outliers upwards

        if "text" in kwargs:
            s_text = kwargs['text']
        if "proz" in kwargs:
            b_percent = kwargs['proz']
        if "abs" in kwargs:
            b_abs = kwargs['abs']
        if "cli" in kwargs:
            b_cli = kwargs['cli']
        if "status" in kwargs:
            s_status = kwargs['status']
        if "abort" in kwargs:
            b_abort = kwargs['abort']
        i_problem = kwargs['problem'] if "problem" in kwargs else None
        s_info = kwargs['info'] if "info" in kwargs else None

        # Spezialfälle abfangen
        if type(i_val_now) == str and i_val_now not in ["on", "pulse", "off"]:
            i_val_now = "pulse"

        if i_val_now == "on":
            if self.is_gui():
                self.__model.get_observer().show_progress(abort=b_abort, pulse=False)
                self.__model.get_observer().set_progress_msg(None, None)
            return
        elif i_val_now == "off":
            if self.is_gui():
                self.__model.get_observer().hide_progress()
                self.__model.get_observer().set_progress_msg(None, None)
            return
        elif i_val_now == "pulse":
            if self.is_gui():
                self.__model.get_observer().show_progress(abort=b_abort, pulse=True)
                self.__model.get_observer().set_progress_msg(None, None)
            return

        # Msg
        if i_problem is not None or s_info is not None:
            if self.is_gui():
                # GUI
                self.__model.get_observer().set_progress_msg(i_problem, s_info)
            else:
                # CLI
                lst_EWI = []
                if i_problem not in (0, None):
                    lst_EWI.append("p"+str(i_problem))
                if s_info not in ("", None):
                    lst_EWI.append(str(s_info))
                if len(lst_EWI) > 0:
                    sEWI = "["+" ".join(lst_EWI)+"] "
                else:
                    sEWI = ""
                s_status = sEWI+s_status

        # Catch outlyer
        if i_val_now > i_val_max:
            i_max_out = i_val_now - i_val_max
            i_val_now = i_val_max
        if i_val_now < 0:
            i_min_out = i_val_now * -1
            i_val_now = 0

        s_progress = self.__get_progress(i_val_now, i_val_max, i_steps)

        if s_progress is not None:

            # Infotext abs val
            s_info_text_abs = " (" + str(i_val_now) + "/" + str(i_val_max) + ")"
            # Out of range max
            if i_max_out != 0:
                s_info_text_abs = " (" + str(i_val_now) + "+" + str(i_max_out) + "!/" + str(i_val_max) + ")"
            elif i_min_out != 0:
                s_info_text_abs = " (0-" + str(i_min_out) + "!/" + str(i_val_max) + ")"

            if self.is_gui():
                # Gui
                s_progress_text = s_text
                if b_percent:
                    s_progress_text += " "+str(s_progress) +"%"
                if b_abs:
                    s_progress_text += s_info_text_abs
                self.__model.get_observer().set_progressbar_proz_show(True) # Prozentwert anzeigen
                self.__model.get_observer().set_progressbar_min_max(1, i_val_max) # Range
                self.__model.get_observer().set_progressbar_val(i_val_now) # Aktueller Wert
                self.__model.get_observer().set_progressbar_text(s_progress_text) # Text
            else:
                # Cli
                if b_cli:
                    if self.__b_last_cmd_progress_cli == False:
                        self.__b_last_cmd_progress_cli = True
                    # if not bAbs:
                    s_info = "Progress" if s_text == "" else s_text
                    i_bar_length = 60
                    f_percent = float(i_val_now) / i_val_max
                    s_arrow = '-' * int(round(f_percent * i_bar_length) - 1) + '>'
                    s_spaces = ' ' * (i_bar_length - len(s_arrow))
                    sys.stdout.write("\r"+s_info+" [{0}] {1}%".format(s_arrow + s_spaces, int(round(f_percent * 100))) + s_status + s_info_text_abs)
                    sys.stdout.flush()

    def __get_progress(self, i_val_now, ival_max, i_steps):
        """
        Determine progress in percent (step by step)
        Args:
            iValueCurrent (int): Current value
            iValueMax (int): Maximum value of the progress scale
            iSteps (int): Number of steps in which the entire scale is to be run through

        Returns:
            int: Progress in percent (0-100%)
        """
        try:
            if i_steps > 100:
                i_steps = 100
            f_steps_rows = ival_max / i_steps
            if i_val_now % f_steps_rows < 1:
                i_percent = int(i_val_now / f_steps_rows / i_steps * 100)
                return i_percent
        except:
            pass
        return None

class FileTools:
    """
    Determine progress in percent (step by step)

        Args:
            iValueCurrent (int): Current value
            iValueMax (int): Maximum value of the progress scale
            iSteps (int): Number of steps in which the entire scale is to be run through

        Returns:
            int: Progress in percent (0-100%)
    """

    @staticmethod
    def prepare_save_file(sFile):
        """
        Checks whether a file already exists and creates a backup if necessary
        """

        result = Result(name="Prepare file to save")

        # File data
        s_f = FileTools.get_file_with_home_expanded(sFile)
        s_f_path = FileTools.get_file_path(s_f)
        s_f_name = FileTools.get_file_name(s_f)
        s_f_type = FileTools.get_file_type(s_f)

        # File Backup?
        if FileTools.is_file_existing(s_f):
            try:
                if s_f_path != "":
                    s_f_path = s_f_path + "/"
                s_f_bak = s_f_path + s_f_name + "_bak_" + PrettyText.create_timestamp() + "." + s_f_type
                os.rename(s_f, s_f_bak)
                result.add_data("bak", s_f_bak)
            except:
                result.add_err("Error during file backup: " + str(traceback.print_exc()))
        return result

    @staticmethod
    def is_file_existing(s_file):
        return os.path.isfile(s_file) if s_file is not None else None

    @staticmethod
    def get_file_with_home_expanded(s_File):
        return os.path.expanduser(s_File) if s_File is not None else None

    @staticmethod
    def get_file_with_abs_path(s_file):
        return os.path.abspath(s_file) if s_file is not None else None

    @staticmethod
    def get_file_without_path(s_file):
        return os.path.basename(s_file) if s_file is not None else None

    @staticmethod
    def get_file_path(s_file):
        return os.path.dirname(s_file) if s_file is not None else None

    @staticmethod
    def get_file_name(s_file):
        if s_file is None:
            return None
        try:
            s_base = os.path.basename(s_file)
            s_name = os.path.splitext(s_base)[0]
            return s_name
        except:
            return None

    @staticmethod
    def get_file_without_type(s_file):
        if s_file is None:
            return None
        try:
            s_file_without_type = os.path.splitext(s_file)[0]
            return s_file_without_type
        except:
            return None

    @staticmethod
    def get_file_type(s_file):
        if s_file is None:
            return None
        sPfad = os.path.splitext(s_file)[1]
        return sPfad[1:]

    @staticmethod
    def create_timestamp():
        s_time_stamp = str(datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S"))
        return s_time_stamp

    @staticmethod
    def is_path_exiting(s_path):
        return os.path.exists(os.path.expanduser(os.path.normpath(s_path))) if s_path is not None else None

    @staticmethod
    def is_path_file(s_path):
        return os.path.isfile(os.path.expanduser(os.path.normpath(s_path))) if s_path is not None else None

    @staticmethod
    def is_path_dir(s_path):
        return os.path.isdir(os.path.expanduser(os.path.normpath(s_path))) if s_path is not None else None

    @staticmethod
    def cut_at_path_tail(s_path):
        if s_path is None:
            return None
        try:
            s_path = os.path.expanduser(os.path.normpath(s_path))
            lst_tmp = os.path.split(s_path)
            s_last = lst_tmp[1]
            s_front = lst_tmp[0]
            return s_front, s_last
        except:
            print("Uuups")
            return None, None


# Pretty Text
class PrettyText:

    def barchart(lst_data, **kwargs):
        """
        Args:
            lst_data: BarName, BarLength
        Kwargs:
             head (Bool: First element in data is header
             maxbarsize (int): Max length of the bars
        Returns:
            str: Bar Chart
        """

        b_html = kwargs["html"] if "html" in kwargs and kwargs["html"] in [True, False] else True
        b_header = kwargs["header"] if "header" in kwargs and kwargs["header"] in [True, False] else True
        i_max_bar_size = kwargs["maxbarsize"] if "maxbarsize" in kwargs else 25

        s_out_header = ""
        s_out = ""
        try:
            if b_header:
                s_head1, s_head2 = lst_data[0]
                del lst_data[0]
            i_max_val = max(count for _, count in lst_data)
            i_increment = i_max_val / i_max_bar_size
            i_longest_label_length = max(len(label) for label, _ in lst_data)
            i_longest_value_str_length = max(len(str(val)) for _, val in lst_data)
            i_bar_max_len = 0
            for i_nr, (s_label, i_count) in enumerate(lst_data):
                i_bar_chunks, i_remainder = divmod(int(i_count * 8 / i_increment), 8)
                s_bar_unicode = '\u2589'
                i_bar = s_bar_unicode * i_bar_chunks
                s_bar = f'{s_label.rjust(i_longest_label_length)} | {str(i_count).rjust(i_longest_value_str_length)} {i_bar}'
                if len(s_bar) > i_bar_max_len:
                    i_bar_max_len = len(s_bar)
                s_out += s_bar + "\n"

            if b_header:
                if len(s_head1) > i_longest_label_length:
                    i_longest_label_length = len(s_head1)
                s_out_header += f'{s_head1.rjust(i_longest_label_length)} | {s_head2}' + "\n"
                s_out_header += i_bar_max_len * "-"
                s_out_header += "\n"
            s_out = s_out_header + s_out
        except Exception as ex:
            print("barchart error: ", str(ex))
        return s_out

    @staticmethod
    def header(s_text, **kwargs):
        """
        args:
            sText: Text
        kwargs:
             char (str): Border letter
             border (i): Border thickness on page
             small (bool): Only one line
        returns
            Text (str)
        """

        if s_text in [None, ""]:
            return ""
        s_char = kwargs.get("char")
        if s_char is None:
            s_char = "*"
        i_border = kwargs.get("border")
        if i_border is None:
            i_border = 0
        if i_border < 0:
            i_border = 0
        b_small = kwargs.get("small")
        if b_small is not True:
            b_small = False

        s_header = ""
        s_header_top_bottom = s_char * (len(s_text) + (2 + 2 * i_border if i_border > 0 else 0)) + "\n"

        if not b_small:
            s_header += s_header_top_bottom
        if i_border > 0:
            s_header += s_char * i_border + " "
        s_header += str(s_text)  # Text
        if i_border > 0:
            s_header += " " + s_char * i_border
        s_header += "\n"
        if not b_small:
            s_header += s_header_top_bottom
        s_header += "\n"
        return s_header

    @staticmethod
    def table(lst, b_header=True, b_html=False, **kwargs):
        """
        Convert list as table

        Args:
             lst (list): List of rows (listen) with columns (str)
             b_header (bool): First line is a header
             b_html (bool): HTML table
        Kwargs:
             header (bool)
             html (bool)
             preview (int): in HTML only preview of x lines
             unify (bool): Bring all lines to the common maximum length, fill each line to the right with None at
        Returns:
            (str): Table
        """

        if "header" in kwargs:
            b_header = kwargs["header"]
        if "html" in kwargs:
            b_html = kwargs["html"]
        i_preview = kwargs["preview"] if "preview" in kwargs else None
        b_unify = kwargs["unify"] if "unify" in kwargs else False
        if type(lst) is set:
            lst = list(lst)
        if b_unify:
            i_max_col = 0
            for lst_row in lst:
                if len(lst_row) > i_max_col:
                    i_max_col = len(lst_row)
            lst_tmp = lst.copy()
            lst = []
            for lst_row in lst_tmp:
                if len(lst_row) < i_max_col:
                    for i in range(i_max_col - len(lst_row)):
                        lst_row.append(None)
                lst.append(lst_row)
        if b_html:
            # HTML
            if i_preview is None:
                # No Preview
                return PrettyText.__table(lst, header=b_header, html=True)
            else:
                # Preview
                i_len_table = len(lst)
                i_preview_table = i_preview
                if b_header:
                    i_len_table -= 1
                    i_preview_table += 1
                if i_preview > 0 and i_preview < i_len_table:
                    s_table_html = ""
                    s_table_html += PrettyText.__table(lst[0: i_preview_table], header=b_header, html=True) + "\n"
                    s_table_html += '<details><summary style="font-size: smaller; color:#000; text-decoration: none;">click here to see remaining ' + str(
                        i_len_table - i_preview) + ' rows ...</summary>\n'
                    lst_rest = lst.copy()
                    i_pop = 0
                    if b_header:
                        i_pop = 1
                    for i in range(i_preview):
                        lst_rest.pop(i_pop)
                    s_table_html += PrettyText.__table(lst_rest, header=b_header, html=True) + "\n"
                    s_table_html += "</details>\n"
                    return s_table_html
                else:
                    return PrettyText.__table(lst, header=b_header, html=True) + "\n"
        else:
            # ASCII
            return PrettyText.__table(lst, header=b_header, html=False)

    @staticmethod
    def __table(lst, **kwargs):
        b_header = kwargs["header"] if "header" in kwargs else False
        b_html = kwargs["html"] if "html" in kwargs else False
        i_preview = kwargs["preview"] if "preview" in kwargs else None
        lst_tmp = []
        for row in lst:
            row_tmp = [str(item) if item is not None else "" for item in row]
            lst_tmp.append(row_tmp)
        lst = lst_tmp
        if b_html:
            s_table = '<table><tbody>'
        else:
            s_table = ""

        i_col_maxlen = [max(map(len, col)) for col in zip(*lst)]

        for nr, row in enumerate(lst):
            if b_html:
                # HTML
                if b_header and nr == 0:
                    # Head
                    s_table += "<tr>"
                    for col in row:
                        s_table += "<td><b>" + str(col) + "</b></td>"
                    s_table += "</tr>"
                else:
                    # Content
                    s_table += "<tr>"
                    for col in row:
                        s_table += "<td>" + str(col) + "</td>"
                    s_table += "</tr>"
            else:
                # ASCII
                # Content
                s_table += " | ".join((val.ljust(Max) for val, Max in zip(row, i_col_maxlen))) + "\n"
                if b_header and nr == 0:
                    s_table += "-|-".join((("-" * Max).ljust(Max) for val, Max in zip(row, i_col_maxlen))) + "\n"
        if b_html:
            s_table += "</tbody></table>"
        else:
            s_table = s_table[:-1]
        return s_table

    @staticmethod
    def lst2str(input, s_separator, i_block_size=None, i_block_items_max=None):
        # List/Dict to String
        if input is None:
            return "-"
        if isinstance(input, str):
            return input
        lst = list()

        # Check block dimensions
        if i_block_items_max is not None and i_block_items_max >= len(input):
            i_block_items_max = None

        if type(input) is dict or type(input) is OrderedDict:
            # Dict/Odict -> List
            lst = [str(k) + ":" + str(v) for k, v in input.items()]
        else:
            # List/Set -> Each element becomes a string
            lst = ["None" if e is None else str(e) for e in input]

        # Special: Blocksize=1 -> Separatior=Return
        if i_block_size is not None:
            if i_block_size == 1:
                i_block_size = None
                s_separator = "\n"
            # Special: Blocksize<=0 -> no Blocks
            elif i_block_size <= 0:
                i_block_size = None

        # Eval: BLocks or not
        if i_block_size is None:
            # No blocks
            s_return = s_separator.join(lst)
        else:
            # Blocks
            # Helper
            def add_item_to_block_row():
                s_item = ""
                if (i + 1) == len(lst):
                    s_sep = ""
                elif (i != 0) and (i % i_block_size) == (i_block_size - 1):
                    s_sep = "\n"
                else:
                    s_sep = f"{s_separator} "
                s_item += f"{s_val}{s_sep}"
                return s_item
            # Blocks without max size
            if i_block_items_max is None:
                # No max block size
                s_return = ""
                for i, s_val in enumerate(lst):
                    s_return += add_item_to_block_row()
            else:
                # Blocks with maximum size
                s_return = ""
                for i, s_val in enumerate(lst[:int(i_block_items_max / 2)]):
                    s_return += add_item_to_block_row()
                s_return += "...\n"
                for i, s_val in enumerate(lst[len(lst)-int(i_block_items_max / 2):]):
                    s_return += add_item_to_block_row()
        return s_return

    @staticmethod
    def lst2strC(lst, i_block_size=None, i_block_items_max=None):
        # List to String (separation by comma)
        return PrettyText.lst2str(lst, ", ", i_block_size, i_block_items_max)

    @staticmethod
    def lst2strR(lst):
        # List to String (separation by comma)
        return PrettyText.lst2str(lst, "\n")

    @staticmethod
    def dic2str(dic, s_sep=", ", i_block_size=None):
        lstOut = [str(k) + "[" + str(v) + "]" for k, v in dic.items()]
        return PrettyText.lst2str(lstOut, s_sep, i_block_size)

    @staticmethod
    def clean_wizard(s_txt, **kwargs):
        """
        Clean Text
        Args:
           s_txt:
        Kwargs:
           no_comments (bool)
           no_whitespaces_front_back (bool)
           max_one_empty_row (bool)
           max_one_space_between_words (bool)
           all_in_one_line (bool)
        Return: Text (str)
        """
        b_no_comments = kwargs["no_comments"] if (
                    "no_comments" in kwargs and kwargs["no_comments"] in [True, False]) else False
        b_no_whitespaces_front_end = kwargs["no_whitespaces_front_back"] if (
                    "no_whitespaces_front_back" in kwargs and kwargs["no_whitespaces_front_back"] in [True,
                                                                                                      False]) else False
        b_max_one_empty_row = kwargs["max_one_empty_row"] if (
                    "max_one_empty_row" in kwargs and kwargs["max_one_empty_row"] in [True, False]) else False
        b_max_one_space_between_words = kwargs["max_one_space_between_words"] if (
                    "max_one_space_between_words" in kwargs and kwargs["max_one_space_between_words"] in [True,
                                                                                                          False]) else False
        b_all_in_one_line = kwargs["all_in_one_line"] if (
                    "all_in_one_line" in kwargs and kwargs["all_in_one_line"] in [True, False]) else False
        if len(kwargs) == 0 or b_all_in_one_line:
            b_no_comments = True
            b_no_whitespaces_front_end = True
            b_max_one_empty_row = True
            b_max_one_space_between_words = True
        if s_txt is None:
            return None
        s_text_return = ""
        for s_row in s_txt.split("\n"):
            if b_no_whitespaces_front_end:
                s_row = s_row.strip()
            if b_no_comments:
                if not (s_row != "" and s_row[0] == "#"):
                    s_text_return += s_row + "\n"
            else:
                s_text_return += s_row + "\n"
        if b_max_one_space_between_words:
            s_text_return = re.sub(r' +', ' ', s_text_return)
        if b_max_one_empty_row:
            s_text_return = re.sub(r'\n\n\n+', '\n\n', s_text_return)
        if b_all_in_one_line:
            s_text_return = re.sub(r'\n+', ' ', s_text_return)
        return s_text_return

    @staticmethod
    def clean_2filename(s_txt, **kwargs):
        bLower = v if (v := kwargs.get("lower")) == False else True
        if s_txt is None:
            return None
        b_umlaut = v if (v := kwargs.get("umlaut")) == False else True
        if s_txt is None:
            return None
        s_txt = s_txt

        # Convert umlauts
        if b_umlaut:
            s_txt = s_txt.replace("ü", 'ue')
            s_txt = s_txt.replace("Ü", 'Ue')
            s_txt = s_txt.replace("ä", 'ae')
            s_txt = s_txt.replace("Ä", 'Ae')
            s_txt = s_txt.replace("ö", 'oe')
            s_txt = s_txt.replace("Ö", 'Oe')
            s_txt = s_txt.replace("ß", 'ss')

        # Everything except alphanumerics -> " "
        s_txt = re.sub('[^0-9a-zA-Z üöäÖÄÜ]+', ' ', s_txt)

        # multi-Space -> 1xSpace
        s_txt = re.sub(' +', ' ', s_txt)

        # Remove whitespace
        s_txt = s_txt.strip()

        # Space -> "_"
        s_txt = re.sub(' ', '_', s_txt)

        # Multi-"_" -> 1x-"_"
        s_txt = re.sub('_+', '_', s_txt)

        # Lower
        if bLower:
            s_txt = s_txt.lower()
        return s_txt

    @staticmethod
    def clean_2ascii_long(s_text):
        try:
            return re.sub(r'[^a-zA-Z0-9,(){}\[\]:,.=?%§!/\-<> ]', "", s_text)
        except:
            return s_text

    @staticmethod
    def clean_del_whitespaces_everyline(s_text):
        try:
            sText_clean = ""
            for sLine in s_text.splitlines():
                sText_clean += sLine.strip() + "\n"
            return sText_clean
        except:
            return s_text

    @staticmethod
    def clean_ret2space(s_text):
        try:
            s_text = PrettyText.clean_2ascii_long(s_text)
            s_text = re.sub(r'[\n]', " ", s_text)
            return s_text
        except:
            return s_text

    @staticmethod
    def duration(i_time_start, **kwargs):
        i_time_end = kwargs["end"] if "end" in kwargs else time.time()
        i_secs = i_time_end - i_time_start
        try:
            i_std = int(i_secs // 3600)
            i_min = int((i_secs % 3600) // 60)
            i_sek = int(i_secs % 60)
            i_msek = int((i_secs % 60) * 1000)
            s_duration = ""
            if i_std > 0:
                s_duration += str(i_std) + "h"
            if i_min > 0:
                s_duration += str(i_min) + "m"
            if i_sek > 0:
                s_duration += str(i_sek) + "s"
            else:
                s_duration += str(i_msek) + "ms"
            return s_duration
        except:
            return ""

    @staticmethod
    def geo_reformat(f_value, **kwargs):
        """
        Georeference reformatting
        Max. Pre-decimal point: 3 digits
        Max. decimal point: 5 digits
            Args:
                f_value (float)
            Kwargs:
                 precision (int): Decimal places to be considered
                 dummy (bool): Fill missing decimal places on
                 dummy_brace (bool): Brackets around decimal places that have been filled
                 dummy_char (str): Dummy character
            return:
                (str): Value
         """

        i_precision_dataset = kwargs.get("precision")
        b_dummy = kwargs.get("dummy")
        b_dummy_brace = kwargs.get("dummy_brace")
        s_dummy_char = kwargs.get("dummy_char")

        i_prec_max = 5
        i_digit_max = 3

        if i_precision_dataset is None:
            i_precision_dataset = 0
        elif i_precision_dataset > i_prec_max:
            i_precision_dataset = i_prec_max
        elif i_precision_dataset < 0:
            i_precision_dataset = 0
        if b_dummy is None:
            b_dummy = True
        if s_dummy_char is None:
            s_dummy_char = "-"
        if b_dummy_brace is None:
            b_dummy_brace = False

        try:
            float(f_value)
        except:
            return str(f_value)

        i_prec_now = 0
        try:
            s_value = str(f_value).strip()
            i_prec_now = len(s_value.split(".")[1])
        except:
            pass

        s_value_formated = str(f_value)
        try:
            if type(f_value) is int:
                f_value = float(f_value)
            i_char_total = i_digit_max + i_precision_dataset
            i_char_total += 1
            if i_precision_dataset > 0:
                i_char_total += 1  # Decimal point (1) -> If decimal point == 0 -> The decimal point is added afterwards ... Eieiei ...
            s_value_formated = f'{f_value:{i_char_total}.{i_precision_dataset}f}'  # TODO: Hier wird leider gerundet (das kann man nicht ausschalten)
            if i_precision_dataset == 0:
                s_value_formated += "."
        except:
            pass
        if b_dummy:
            i_digits_dummy = i_prec_max - i_precision_dataset
            if i_digits_dummy > 0:
                s_value_formated += ("[" if b_dummy_brace else "") + (s_dummy_char * i_digits_dummy) + (
                    "]" if b_dummy_brace else "")
        return s_value_formated

    @staticmethod
    # Version a name
    def versionize(s_name, lst_ref):
        if s_name is None:
            s_name = "no_name"
        s_name_versionized = s_name
        if s_name not in lst_ref:
            return s_name
        b_loop = True
        i = 0
        while b_loop:
            i += 1
            s_name_versionized = str(s_name + "_" + str(i))
            if s_name_versionized not in lst_ref:
                b_loop = False
            if i > 100:
                s_name_versionized = s_name + "_" + PrettyText.create_timestamp()
                b_loop = False
        return s_name_versionized

    @staticmethod
    def create_timestamp(**kwargs):
        """
        Create Time stamp
            kwargs: mode (str): normal, normal_date_only, file (default)
            returns: (str) timestamp
        """

        s_mode = kwargs.get("mode")
        s_format = ""
        if s_mode == "normal":
            s_format = "%Y-%m-%d %H:%M:%S"
        elif s_mode == "normal_date_only":
            s_format = "%Y-%m-%d"
        elif s_format == "file":
            s_format = "%Y-%m-%d_%H-%M-%S"
        else:
            s_format = "%Y-%m-%d_%H-%M-%S"  # Default: File
        s_time_stamp = str(datetime.datetime.now().strftime(s_format))
        return s_time_stamp

    @staticmethod
    def percent2str(i_precent_val, i_base_val, **kwargs):
        """
        Display percentage value, basic value as string
        args:
             iProzentwert (int): Percentage value
             iBasic value (int): Basic value
        kwargs:
            justify (bool) (std:true): Always increase space to 4 digits
            show_none (bool) (std:false): Show for None: "-"
        """

        b_none = v if (v := kwargs.get("show_none")) == True else False
        b_just = v if (v := kwargs.get("justify")) == False else True

        if i_base_val == 0:
            f_proz = None
        elif i_base_val != 0:
            f_proz = (i_precent_val / i_base_val) * 100
        else:
            f_proz = 0
        return PrettyText.precentf2str(f_proz, show_none=b_none, justify=b_just)

    @staticmethod
    def precentf2str(f_proz, **kwargs):
        """
        Display percent as string
        args:
            fPRoz (float): PRozent
        kwargs:
            justify (bool) (std:true): Always increase space to 4 digits
            show_none (bool) (std:false): If None - show
        """

        b_none = v if (v := kwargs.get("show_none")) == True else False
        b_just = v if (v := kwargs.get("justify")) == False else True
        s_proz = ""

        try:
            f_proz = float(f_proz)
        except:
            f_proz = None
        if f_proz is not None:
            # Out of range
            if f_proz < 0:
                f_proz = 0
            if f_proz > 100:
                f_proz = 100

            if f_proz < 1 and f_proz > 0:
                s_proz = "<1%"
            else:
                s_proz = f"{int(f_proz)}%"
        if f_proz is None:
            if b_none:
                s_proz = "-"
            else:
                return ""
        if b_just:
            s_proz = s_proz.rjust(4)
        return s_proz

    @staticmethod
    def id_generator(i_size=6):
        chars = string.ascii_uppercase + string.digits
        return ''.join(random.choice(chars) for _ in range(i_size))
