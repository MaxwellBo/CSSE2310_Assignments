#!/usr/bin/env python
from __future__ import print_function
import sys
import os

# Get correct path to files, based on platform
import platform
host = platform.node().split('.')[0]

sys.path[0:0]=['/local/courses/csse2310/lib']
TEST_LOCATION = '/home/users/uqjfenw1/public/a3p'
import marks
import shutil
COMPILE = "make"
 
 
class Assignment3(marks.TestCase):
  timeout = 12 
  @classmethod
  def setup_class(cls):
        # Store original location
        options = getattr(cls, '__marks_options__', {})
	cls.progplayer = os.path.join(options['working_dir'], './EAIT')
	cls.progplayersci = os.path.join(options['working_dir'], './SCIENCE')
	cls.progplayerhabs = os.path.join(options['working_dir'], './HABS')
	cls.progplayermabs = os.path.join(options['working_dir'], './MABS')
	cls.progplayerhass = os.path.join(options['working_dir'], './HASS')	
        cls.progplayerren = os.path.join(options['working_dir'], 'deleteme')
        cls.proghub = os.path.join(options['working_dir'], './stlucia')
        cls._compile_warnings = 0
        cls._compile_errors = 0
 
        # Create symlink to tests in working dir
        os.chdir(options['working_dir'])
        try:
            os.symlink(TEST_LOCATION, 'tests')
        except OSError:
            pass
        os.chdir(options['temp_dir'])

        # Modify test environment when running tests (excl. explain mode).
        if not options.get('explain', False):
            # Compile program
            os.chdir(options['working_dir'])
            p = marks.Process(COMPILE.split())
            os.chdir(options['temp_dir'])
 
            # Count warnings and errors
            warnings = 0
            errors = 0
            while True:
                line = p.readline_stderr()
                if line == '':
                    break
                if 'warning:' in line:
                    warnings += 1
                if 'error:' in line:
                    errors += 1
                print(line, end='')

            # Do not run tests if compilation failed.
            assert p.assert_exit_status(0)
 
            # Create symlink to tests within temp folder
            try:
                os.symlink(TEST_LOCATION, 'tests')
            except OSError:
                pass
            # Add some copies
            try:
                shutil.copyfile(cls.progplayer, cls.progplayerren)
                shutil.copymode(cls.progplayer, cls.progplayerren)
            except:
                pass # Tests will fail later

#bad arguments
  @marks.marks('pArgument checking', category_marks=2)
  def test_pargs1(self):
    """ """
    p = self.process([self.progplayer], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/pusage.err')
    self.assert_exit_status(p, 1)

  @marks.marks('pArgument checking', category_marks=2)
  def test_pargs2(self):
    """ """
    p = self.process([self.progplayer, '4'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/pusage.err')
    self.assert_exit_status(p, 1)

  @marks.marks('pArgument checking', category_marks=2)
  def test_pargs3(self):
    """ """
    p = self.process([self.progplayer, '0', 'B'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/cpnum.err')
    self.assert_exit_status(p, 2)

  @marks.marks('pArgument checking', category_marks=2)
  def test_pargs4(self):
    """ """
    p = self.process([self.progplayer, '4', ';'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/cpid.err')
    self.assert_exit_status(p, 3)   

#hubloss and end message handled correctly

  @marks.marks('Hubloss', category_marks=2)
  def test_hubandend1(self):
    """ """
    p = self.process([self.progplayer, '4', 'B'], 'tests/sd1.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/sd1.out')
    self.assert_stderr_matches_file(p, 'tests/sd1.err')
    self.assert_exit_status(p, 0)

#Correct choice for initial move

  @marks.marks('initial move', category_marks=2)
  def test_fm1(self):
    """ """
    p = self.process([self.progplayer, '3', 'B'], 'tests/fm1.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/fm1.out')
    self.assert_stderr_matches_file(p, 'tests/fm1.err')
    self.assert_exit_status(p, 4)

  @marks.marks('initial move', category_marks=2)
  def test_fm2(self):
    """ """
    p = self.process([self.progplayer, '4', 'A'], 'tests/fm2.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/fm2.out')
    self.assert_stderr_matches_file(p, 'tests/fm2.err')
    self.assert_exit_status(p, 4)

  @marks.marks('initial move', category_marks=2)
  def test_fm3(self):
    """ """
    p = self.process([self.progplayer, '4', 'A'], 'tests/fm3.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/fm3.out')
    self.assert_stderr_matches_file(p, 'tests/fm3.err')
    self.assert_exit_status(p, 4)

#Correctly handle complete game
  @marks.marks('playgame', category_marks=4)
  def test_pg1(self):
    """ """
    p = self.process([self.progplayer, '2', 'A'], 'tests/pg1.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/pg1.out')
    self.assert_stderr_matches_file(p, 'tests/pg1.err')
    self.assert_exit_status(p, 0)

#Detect invalid messages

  @marks.marks('bad message', category_marks=2)
  def test_badm1(self):
    """ """
    p = self.process([self.progplayer, '2', 'A'], 'tests/bh1.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/bh1.out')
    self.assert_stderr_matches_file(p, 'tests/bh1.err')
    self.assert_exit_status(p, 5)

  @marks.marks('bad message', category_marks=2)
  def test_badm2(self):
    """ """
    p = self.process([self.progplayer, '2', 'A'], 'tests/bh2.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/bh2.out')
    self.assert_stderr_matches_file(p, 'tests/bh2.err')
    self.assert_exit_status(p, 5)

  @marks.marks('bad message', category_marks=2)
  def test_badm3(self):
    """ """
    p = self.process([self.progplayer, '3', 'C'], 'tests/bh3.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/bh3.out')
    self.assert_stderr_matches_file(p, 'tests/bh3.err')
    self.assert_exit_status(p, 5)

  @marks.marks('bad message', category_marks=2)
  def test_badm5(self):
    """ """
    p = self.process([self.progplayer, '3', 'A'], 'tests/bh5.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/bh5.out')
    self.assert_stderr_matches_file(p, 'tests/bh5.err')
    self.assert_exit_status(p, 5)

  @marks.marks('bad message', category_marks=2)
  def test_badm4(self):
    """ """
    p = self.process([self.progplayer, '2', 'A'], 'tests/bh4.in' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/bh4.out')
    self.assert_stderr_matches_file(p, 'tests/bh4.err')
    self.assert_exit_status(p, 5)

# stlucia arg checking
  @marks.marks('stlucia arg checking', category_marks=2)
  def test_hargc1(self):
    """ """
    p = self.process([self.proghub], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/usage.err')
    self.assert_exit_status(p, 1)
  
  @marks.marks('stlucia arg checking', category_marks=2)
  def test_hargc2(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/usage.err')
    self.assert_exit_status(p, 1)
  
  @marks.marks('stlucia arg checking', category_marks=2)
  def test_hargc3(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '3', './EAIT'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/usage.err')
    self.assert_exit_status(p, 1)
  
  @marks.marks('stlucia arg checking', category_marks=2)
  def test_hargc4(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '-5', './EAIT', './EAIT'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/score.err')
    self.assert_exit_status(p, 2)

  @marks.marks('stlucia arg checking', category_marks=2)
  def test_hargc5(self):
    """ """
    p = self.process([self.proghub]+['tests/q.roll', '4', './EAIT', './EAIT'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/access.err')
    self.assert_exit_status(p, 3)
  
  @marks.marks('stlucia arg checking', category_marks=2)
  def test_hargc6(self):
    """ """
    p = self.process([self.proghub]+['tests/y.roll', '3', './EAIT', './EAIT'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/rollf.err')
    self.assert_exit_status(p, 4)
     
  @marks.marks('Failure to start players', category_marks=2)
  def test_startfail1(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '10', './EAIT', 'qsqsq'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/fork.err')
    self.assert_exit_status(p, 5)  
  

#Correctly handle 2 player games using a single deck
  @marks.marks('1 roll', category_marks=4)
  def test_1deck0(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '10', self.progplayerren, self.progplayerren], 'tests/empty' )
    p.finish_input()
    self.assert_stderr_matches_file(p, 'tests/3.2.1.0.err')
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)

  @marks.marks('1 roll', category_marks=4)
  def test_1deck1(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '10', self.progplayer, self.progplayer], 'tests/empty' )
    p.finish_input()
    self.assert_stderr_matches_file(p, 'tests/3.2.1.err')
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)

  @marks.marks('1 roll', category_marks=4)
  def test_1deck5(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '10', self.progplayer, self.progplayer], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/3.2.2.out')
    self.assert_exit_status(p, 0)


#Play complete games with 3 sci players

  @marks.marks('3sci players', category_marks=2)
  def test_2play1(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '10', self.progplayersci, self.progplayersci, self.progplayersci], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/2p1out')
    self.assert_exit_status(p, 0)

  @marks.marks('3sci players', category_marks=2)
  def test_2play2(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '20', self.progplayersci, self.progplayersci, self.progplayersci], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/2p2out')
    self.assert_exit_status(p, 0)


  @marks.marks('3sci players', category_marks=2)
  def test_2play3(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '20', self.progplayersci, self.progplayersci, self.progplayersci], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/2p3out')
    self.assert_exit_status(p, 0)

#play 4 HABS

  @marks.marks('4habs players', category_marks=2)
  def test_4play1(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '11', self.progplayerhabs, self.progplayerhabs, self.progplayerhabs, self.progplayerhabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/4p1out')
    self.assert_exit_status(p, 0)

  @marks.marks('4habs players', category_marks=2)
  def test_4play2(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '19', self.progplayerhabs, self.progplayerhabs, self.progplayerhabs, self.progplayerhabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/4p2out')
    self.assert_exit_status(p, 0)

  @marks.marks('4habs players', category_marks=2)
  def test_4play5(self):
    """ """
    p = self.process([self.proghub]+['tests/pp.roll', '100', self.progplayerhabs, self.progplayerhabs, self.progplayerhabs, self.progplayerhabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/4p5out')
    self.assert_exit_status(p, 0)

# 5 mabs players

  @marks.marks('5mabs players', category_marks=2)
  def test_5play1(self):
    """ """
    p = self.process([self.proghub]+['tests/pp.roll', '11', self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayermabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/5p1out')
    self.assert_exit_status(p, 0)

  @marks.marks('5mabs players', category_marks=2)
  def test_5play2(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '19', self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayermabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/5p2out')
    self.assert_exit_status(p, 0)


  @marks.marks('5mabs players', category_marks=2)
  def test_5play3(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '40', self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayermabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/5p3out')
    self.assert_exit_status(p, 0)

#complete games
  @marks.marks('complete', category_marks=12)
  def test_comp1(self):
    """ """
    p = self.process([self.proghub]+['tests/pp.roll', '11', self.progplayermabs, self.progplayerhass, self.progplayerhass], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/cp1out')
    self.assert_exit_status(p, 0)

  @marks.marks('complete', category_marks=12)
  def test_comp2(self):
    """ """
    p = self.process([self.proghub]+['tests/x.roll', '11', self.progplayerhabs, self.progplayerhass, self.progplayerhass, self.progplayersci], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/cp2out')
    self.assert_exit_status(p, 0)

  @marks.marks('complete', category_marks=12)
  def test_comp3(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '11', self.progplayerhabs, self.progplayerhass, self.progplayersci, self.progplayer], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/cp3out')
    self.assert_exit_status(p, 0)

  @marks.marks('complete', category_marks=12)
  def test_comp4(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '19', self.progplayerhabs, self.progplayerhass, self.progplayersci, self.progplayer, self.progplayerhabs, self.progplayerhabs, self.progplayerhabs, self.progplayerhabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/cp4out')
    self.assert_exit_status(p, 0)

  @marks.marks('complete', category_marks=12)
  def test_comp5(self):
    """ """
    p = self.process([self.proghub]+['tests/z.roll', '19', self.progplayer, self.progplayer, self.progplayersci, self.progplayer, self.progplayerhabs, self.progplayerhabs, self.progplayer, self.progplayerhabs], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/cp5out')
    self.assert_exit_status(p, 0)

#  @marks.marks('complete', category_marks=12)
#  def test_comp6(self):
#    """ """
#    p = self.process([self.proghub]+['tests/z.roll', '100', self.progplayer, self.progplayer, self.progplayer, self.progplayer, self.progplayerhabs, self.progplayerhabs, self.progplayer, self.progplayerhabs, self.progplayersci, self.progplayersci, self.progplayersci, self.progplayersci, self.progplayerhabs, self.progplayer, self.progplayer], 'tests/empty' )
#   p.finish_input()
#   self.assert_stdout_matches_file(p, 'tests/empty')
#   self.assert_stderr_matches_file(p, 'tests/cp6out')
#    self.assert_exit_status(p, 0)


#  @marks.marks('complete', category_marks=12)
#  def test_comp7(self):
#    """ """
#    p = self.process([self.proghub]+['tests/z.roll', '103', self.progplayersci, self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayermabs, self.progplayerhass, self.progplayer, self.progplayer, self.progplayer, self.progplayer, self.progplayerhabs, self.progplayerhabs, self.progplayer, self.progplayerhabs, self.progplayersci, self.progplayersci, self.progplayersci, self.progplayersci, self.progplayerhabs, self.progplayer, self.progplayer], 'tests/empty' )
#    p.finish_input()
#    self.assert_stdout_matches_file(p, 'tests/empty')
#    self.assert_stderr_matches_file(p, 'tests/cp7out')
#    self.assert_exit_status(p, 0)
    
#  @marks.marks('complete', category_marks=12)
#  def test_comp8(self):
#    """ """
#    p = self.process([self.proghub]+['tests/z.roll', '103',
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs,
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs,
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs,
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs,
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs,
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs,
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs,
#self.progplayerhabs, self.progplayerhabs, self.progplayerhabs
#], 'tests/empty' )
#    p.finish_input()
#    self.assert_stdout_matches_file(p, 'tests/empty')
#    self.assert_stderr_matches_file(p, 'tests/cp8out')
#    self.assert_exit_status(p, 0)

if __name__ == '__main__':
    marks.main()

