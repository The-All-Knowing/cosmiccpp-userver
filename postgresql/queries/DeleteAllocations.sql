DELETE FROM allocation.allocations l
USING allocation.batches b
WHERE l.batch_id = b.id
AND b.reference IN ($1)
RETURNING l.orderline_id;