#include <gtest/gtest.h>

TEST(QueueTest, NewQueue) {
    queue *q = new_queue(sizeof(int), 10);
    ASSERT_FALSE(queue_full(q));
    ASSERT_EQ(0, q->queue_size);
    ASSERT_EQ(10, q->queue_capacity);
    ASSERT_EQ(sizeof(int), q->element_size);
}

TEST(QueueTest, EnqueueAndDequeue) {
    queue *q = new_queue(sizeof(int), 10);
    int *in = (int *)queue_enqueue(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));

    int *out = (int *)queue_dequeue(q);
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
}

TEST(QueueTest, FillQueue) {
    queue *q = new_queue(sizeof(int), 5);
    for (int i = 0; i < 5; i++) {
        int *value = (int *)queue_enqueue(q);
        memcpy(value, &i, sizeof(int));
    }

    ASSERT_TRUE(queue_full(q));
}

TEST(QueueTest, ExtendQueue) {
    queue *q = new_queue(sizeof(int), 5);
    queue_extend(q);
    ASSERT_EQ(10, q->queue_capacity);
    ASSERT_EQ(0, q->queue_size);
}

TEST(QueueTest, OverflowExtendQueue) {
    queue *q = new_queue(sizeof(int), 5);
    for (int i = 0; i < 6; i++) {
        int *value = (int *)queue_enqueue(q);
        memcpy(value, &i, sizeof(int));
    }

    ASSERT_EQ(10, q->queue_capacity);
    ASSERT_EQ(6, q->queue_size); 
}

TEST(QueueTest, DequeueOrder) {
    queue *q = new_queue(sizeof(int), 5);
    for (int i = 0; i < 100; i++) {
        int *value = (int *)queue_enqueue(q);
        memcpy(value, &i, sizeof(int));
    }

    ASSERT_EQ(160, q->queue_capacity);
    ASSERT_EQ(100, q->queue_size);
    for(int i = 0; i < 100; i++) {
        int *value = (int *)queue_dequeue(q);
        ASSERT_EQ(i, *value);
    }
}