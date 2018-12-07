report.pdf: $(wildcard *.tex) $(wildcard *.bit) $(wildcard *.cls) $(wildcard *.bbx) $(wildcard *.bst) $(wildcard *.cbx) $(wildcard *.dbx) $(wildcard *.png)
	latexmk -pdf report.tex < /dev/null

clean:
	latexmk -c

