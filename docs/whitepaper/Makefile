# Build the manuscript using checked-in evidence; do not rerun experiments.
BUILD := $(abspath .build)
LATEXMK ?= latexmk

.PHONY: all data clean

all:
	mkdir -p "$(BUILD)"
	$(LATEXMK) -pdf -interaction=nonstopmode -halt-on-error -file-line-error \
		-bibtexfudge- \
		-pdflatex='pdflatex -no-shell-escape %O %S' \
		-jobname=limit-order-book -outdir="$(BUILD)" main.tex

data:
	python3 evaluate.py --summarize evidence

clean:
	$(LATEXMK) -C -jobname=limit-order-book -outdir="$(BUILD)" main.tex
