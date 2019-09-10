import tensorflow as tf
from tensorflow import keras


def model_sign():
    inputs = keras.layers.Input(shape=(32, 32, 1))
    x = keras.layers.Conv2D(32, kernel_size=5, padding='same', activation=tf.nn.relu)(inputs)
    x = keras.layers.MaxPool2D(pool_size=(2, 2), strides=2)(x)
    x = keras.layers.Dropout(0.1)(x)
    part1 = keras.layers.Flatten()(keras.layers.MaxPool2D(pool_size=(4, 4), strides=4)(x))

    x2 = keras.layers.Conv2D(64, kernel_size=5, padding='same', activation=tf.nn.relu)(x)
    x2 = keras.layers.MaxPool2D(pool_size=(2, 2), strides=2)(x2)
    x2 = keras.layers.Dropout(0.2)(x2)
    part2 = keras.layers.Flatten()(keras.layers.MaxPool2D(pool_size=(2, 2), strides=2)(x2))

    x3 = keras.layers.Conv2D(128, kernel_size=5, padding='same', activation=tf.nn.relu)(x2)
    x3 = keras.layers.MaxPool2D(pool_size=(2, 2), strides=2)(x3)
    x3 = keras.layers.Dropout(0.3)(x3)
    part3 = keras.layers.Flatten()(x3)

    xfc = keras.layers.concatenate([part1, part2, part3])

    xfc = keras.layers.Dense(1024, activation=tf.nn.relu)(xfc)
    xfc = keras.layers.Dropout(0.5)(xfc)
    xfc = keras.layers.Dense(3, activation=tf.nn.softmax)(xfc)

    model = keras.Model(inputs=inputs, outputs=xfc)
    return model