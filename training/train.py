#!/usr/bin/env python3
"""Train a small, inspectable GPAI topic classifier with multinomial Naive Bayes."""

from __future__ import annotations

import argparse
import json
import math
import re
import unicodedata
from collections import Counter, defaultdict
from pathlib import Path

TOKEN_RE = re.compile(r"[\uac00-\ud7a3]+|[a-z]+|[0-9]+(?:\.[0-9]+)?", re.IGNORECASE)


def features(text: str) -> list[str]:
    normalized = unicodedata.normalize("NFKC", text).lower()
    result: list[str] = []
    for word in TOKEN_RE.findall(normalized):
        result.append("w:" + word)
        if all("\uac00" <= char <= "\ud7a3" for char in word):
            for width in (2, 3):
                result.extend(
                    f"c{width}:{word[index:index + width]}"
                    for index in range(len(word) - width + 1)
                )
    return result


def fit(rows: list[dict[str, str]], labels: list[str], alpha: float = 0.5) -> dict:
    class_counts = Counter(row["label"] for row in rows)
    token_counts = {label: Counter() for label in labels}
    totals = Counter()
    for row in rows:
        tokens = features(row["text"])
        token_counts[row["label"]].update(tokens)
        totals[row["label"]] += len(tokens)

    vocabulary = sorted({token for counts in token_counts.values() for token in counts})
    denominator_extra = alpha * (len(vocabulary) + 1)
    log_prior = {
        label: math.log(class_counts[label] / len(rows))
        for label in labels
    }
    log_likelihood = {}
    for label in labels:
        denominator = totals[label] + denominator_extra
        values = {
            token: math.log((token_counts[label][token] + alpha) / denominator)
            for token in vocabulary
        }
        values["<UNK>"] = math.log(alpha / denominator)
        log_likelihood[label] = values
    return {
        "labels": labels,
        "vocabulary": vocabulary,
        "logPrior": log_prior,
        "logLikelihood": log_likelihood,
    }


def predict(model: dict, text: str) -> str:
    known = set(model["vocabulary"])
    scores = {}
    tokens = features(text)
    for label in model["labels"]:
        likelihood = model["logLikelihood"][label]
        scores[label] = model["logPrior"][label] + sum(
            likelihood.get(token, likelihood["<UNK>"])
            for token in tokens
            if token in known
        )
    return max(scores, key=scores.get)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--data", default="training/examples.json")
    parser.add_argument("--output", default="dist/assets/domain-model.json")
    args = parser.parse_args()

    rows = json.loads(Path(args.data).read_text(encoding="utf-8"))
    labels = sorted({row["label"] for row in rows})
    if len(labels) != 4:
        raise ValueError(f"Expected four subject labels, found {len(labels)}")

    # Deterministic stratified holdout: every fifth example per label is validation data.
    train_rows: list[dict[str, str]] = []
    validation_rows: list[dict[str, str]] = []
    seen = Counter()
    for row in rows:
        index = seen[row["label"]]
        seen[row["label"]] += 1
        (validation_rows if index % 5 == 0 else train_rows).append(row)

    validation_model = fit(train_rows, labels)
    correct = sum(
        predict(validation_model, row["text"]) == row["label"]
        for row in validation_rows
    )
    accuracy = correct / len(validation_rows) if validation_rows else 0.0

    # Export a final model trained on the complete curated corpus.
    model = fit(rows, labels)
    model.update({
        "formatVersion": 1,
        "trainingExamples": len(rows),
        "validation": {
            "examples": len(validation_rows),
            "correct": correct,
            "accuracy": accuracy,
            "split": "deterministic, every fifth example per label",
        },
        "notice": (
            "Small educational topic classifier trained on the included examples. "
            "Scores are not calibrated probabilities and it does not solve problems."
        ),
    })
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(
        json.dumps(model, ensure_ascii=False, separators=(",", ":")) + "\n",
        encoding="utf-8",
    )
    print(
        f"Topic classifier: {len(rows)} examples; validation "
        f"{correct}/{len(validation_rows)} = {accuracy:.1%}; "
        f"{len(model['vocabulary'])} features"
    )


if __name__ == "__main__":
    main()
