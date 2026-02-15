DELETE FROM allocation.batches
WHERE reference IN ($1);