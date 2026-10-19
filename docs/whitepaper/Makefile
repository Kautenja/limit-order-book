# Build the manuscript using checked-in evidence; do not rerun experiments.
ROOT := ../..
BUILD := $(abspath $(ROOT)/.build/report)
LATEXMK ?= latexmk

.PHONY: all data clean

all:
	mkdir -p "$(BUILD)"
	$(LATEXMK) -pdf -interaction=nonstopmode -halt-on-error -file-line-error \
		-pdflatex='pdflatex -no-shell-escape %O %S' \
		-jobname=limit-order-book -outdir="$(BUILD)" main.tex

data:
	python3 evaluate.py --summarize evidence

clean:
	$(LATEXMK) -C -jobname=limit-order-book -outdir="$(BUILD)" main.tex
