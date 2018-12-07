\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{homework}
\LoadClass[10pt]{article}

%% WHERE TO START
%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%

% Quick start:
% 1) Download this file and copy it into the directory where your .TeX file is.
% 2) Add \LoadClass{homework} to the top of your .TeX file.

%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%

%% YOUR INFO HERE
%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%
\author{Andrew Dunne (asd222), Alex Katz (aik49), Daniel Wisner (daw268), Jacob Glueck (jng55)}

\newcommand{\class}{ECE 5775: High-Level Digital Design Automation}
\newcommand{\homework}[1]{\title{Homework #1}}
\newcommand{\lab}[2]{\title{Lab #1: #2}}
\newcommand{\project}[1]{\title{#1}}

%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%

%% Helper Packages
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\RequirePackage{fancyhdr}
\RequirePackage[usenames]{color}
\RequirePackage{alltt}
\RequirePackage[left=.8in,top=.8in,right=1in,bottom=1in]{geometry}
\RequirePackage{parskip}
\RequirePackage{xstring}
\RequirePackage{comment}
\RequirePackage{amsmath}
\RequirePackage{amsthm}
\RequirePackage{amsfonts}
\RequirePackage{multicol}
\RequirePackage{amssymb}
\RequirePackage{enumitem}
\RequirePackage{multicol}
\RequirePackage{xcolor}
\RequirePackage[scientific-notation=true]{siunitx}
\RequirePackage{setspace}
\RequirePackage{tikz}
\RequirePackage{listings}
\RequirePackage{color}
\RequirePackage{algorithm}
% \usepackage{algorithmicx}
\RequirePackage[noend]{algpseudocode}
\RequirePackage{siunitx}
\RequirePackage{booktabs}
\RequirePackage{shortvrb}


%% Personalized notation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\MakeShortVerb"

\graphicspath{ {images/} }

%probability
\newcommand{\prob}[1]{\mathbf P({#1})}
%expected value
\newcommand{\expec}[1]{\mathbf E[#1]}
%variance
\newcommand{\var}[1]{\text {var($#1$)}}
%square of expected value
\newcommand{\expsq}[1]{\mathbf E^2[#1]}

%norms
\newcommand{\norm}[1]{\left\lVert#1\right\rVert}

%numbering equations inside {align*}
\newcommand\numberthis{\addtocounter{equation}{1}\tag{\theequation}}

\newcommand{\nth}{n^{th}}

%vectors
\renewcommand{\vec}[1]{\mathbf{#1}}

%Shortcuts for proof by induction
\newcommand{\ih}{\tag*{\textsc {[ind. hypothesis]}}}
\newcommand{\bc}{\textsc {\\basis: }}
\newcommand{\is}{\textsc {\\inductive step: }}
\newcommand{\case}[1]{\textsc {case #1:}}
%Commonly used symbols
\newcommand{\N}{\mathbb{N}}
\newcommand{\R}{\mathbb{R}}
\newcommand{\Q}{\mathbb{Q}}
\newcommand{\Z}{\mathbb{Z}}
\renewcommand{\S}{\mathcal{S}}
\renewcommand{\P}{\mathcal{P}}
\newcommand{\Var}{\mathrm{Var}}
\newcommand{\Exp}{\mathrm{E}}

%Red highlight
\newcommand{\rhighlight}[1]{\colorbox{red!50}{$\displaystyle#1$}}
%Yellow highlight
\newcommand{\yhighlight}[1]{\colorbox{yellow!50}{$\displaystyle#1$}}
%highlighting problems
\newcommand{\TODO}[1][fix]{\rhighlight{\textbf{[TODO --- #1]}}}

\DeclareMathOperator*{\argmin}{argmin}
\DeclareMathOperator*{\argmax}{argmax}

%% code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\definecolor{dkgreen}{rgb}{0,0.6,0}
\definecolor{gray}{rgb}{0.5,0.5,0.5}
\definecolor{mauve}{rgb}{0.58,0,0.82}

\lstset{
  language=Python,
  numbers=left,
  stepnumber=1,
  firstnumber=1,
  numberfirstline=true
}

%% Text Settings
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\setlength{\parindent}{0pt} % just skips, no indent
\setlength{\parskip}{3pt}
% \setlist[enumerate]{leftmargin=*}
% \linespread{1}

%% Header %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\renewcommand\@maketitle{
    \begin{spacing}{1}
        \begin{center}
            \rule{\linewidth}{1pt}
            \large\textbf{\@title}\\
            \class\\
            {\small \@author\\
            \today}\\[-0.5em]
            \rule{\linewidth}{1pt}
        \end{center}
   \end{spacing}
  \bigskip
}

%% Algorithms
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\algdef{SE}[DOWHILE]{Do}{doWhile}{\algorithmicdo}[1]{\algorithmicwhile\ #1}%
\algnewcommand{\True}{\textit{true}}
\algnewcommand{\False}{\textit{false}}
